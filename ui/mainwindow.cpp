#include "mainwindow.h"

#include <QAbstractItemView>
#include <QCoreApplication>
#include <QDateTime>
#include <QDir>
#include <QFileInfo>
#include <QHeaderView>
#include <QLabel>
#include <QSignalBlocker>
#include <QTableWidgetItem>
#include <QStringList>
#include <chrono>
#include <iostream>
#include "ui_mainwindow.h"
#include "../external/sqlite/sqlite3.h"

namespace
{
// Just gets the current time in milliseconds so the status math can stay simple.
qint64 NowMs()
{
    auto now = std::chrono::system_clock::now().time_since_epoch();
    return std::chrono::duration_cast<std::chrono::milliseconds>(now).count();
}

// Turns "how stale is this packet" into a small operator-friendly status word.
QString GetLinkStatus(qint64 ageMs)
{
    if (ageMs < 3000)
    {
        return "Connected";
    }

    if (ageMs <= 7000)
    {
        return "Degraded";
    }

    return "Disconnected";
}

// Tiny battery grading helper so the UI can say more than just a number.
QString GetBatteryStatus(double battery)
{
    if (battery < 30.0)
    {
        return "Critical";
    }

    if (battery <= 70.0)
    {
        return "Normal";
    }

    return "Good";
}

// Same idea but for temperature, because "44.2" by itself does not tell much of a story.
QString GetTempStatus(double temperature)
{
    if (temperature < 0.0)
    {
        return "Critical Low";
    }

    if (temperature < 15.0)
    {
        return "Low";
    }

    if (temperature <= 60.0)
    {
        return "Normal";
    }

    if (temperature <= 75.0)
    {
        return "High";
    }

    return "Critical High";
}

// This is the lazy summary line that squashes a few health checks into one word.
QString GetHealthStatus(const QString& linkStatus, const QString& batteryStatus, const QString& temperatureStatus)
{
    if (linkStatus == "Disconnected")
    {
        return "Offline";
    }

    if (batteryStatus.contains("Critical") || temperatureStatus.contains("Critical"))
    {
        return "Attention";
    }

    if (linkStatus == "Degraded" || temperatureStatus == "High")
    {
        return "Warning";
    }

    return "Nominal";
}

// Makes the raw millisecond timestamp look like something a human can read.
QString FormatTimeText(qint64 epochMs)
{
    return QDateTime::fromMSecsSinceEpoch(epochMs).toString("yyyy-MM-dd hh:mm:ss");
}

// We do not store packet size in the DB yet, so this is our best educated guess for now.
QString GuessPacketSize(const QString& satName, qint64 sequence, qint64 timestampMs, double battery, double temperature)
{
    // No packet-size column lives in SQLite yet, so we estimate the payload size from the frame shape we already use.
    const QString payload = QString("{\"sat_id\":\"%1\",\"sequence\":%2,\"timestamp_ms\":%3,\"battery\":%4,\"temp_c\":%5}")
        .arg(satName)
        .arg(sequence)
        .arg(timestampMs)
        .arg(QString::number(battery, 'f', 2))
        .arg(QString::number(temperature, 'f', 2));

    return QString::number(payload.toUtf8().size()) + " B";
}

// Clears the detail labels when nothing useful is selected or connected.
void ClearDetailLabels(Ui::mainwindow* ui)
{
    ui->SatelliteName_Label->setText("N/A");
    ui->LinkStatus_Connection->setText("Disconnected");
    ui->PacketLoss_Connection->setText("N/A");
    ui->Latency_Data->setText("N/A");
    ui->Battery_Data->setText("N/A");
    ui->Temprature_Data->setText("N/A");
    ui->Battery_StatusData->setText("N/A");
    ui->Temp_StatusData->setText("N/A");
    ui->Health_StatusData->setText("N/A");
}
}

mainwindow::mainwindow(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::mainwindow)
{
    ui->setupUi(this);

    // Build the two tables first so the rest of the refresh code has a sane place to drop data.
    ConfigureTelemetryTable();
    ConfigureSatelliteTable();
    StyleTheUi();

    // This label is our "hey nothing is connected right now" message.
    noSatelliteLabel = new QLabel("No active satellite connection", this);
    noSatelliteLabel->setGeometry(16, 20, 260, 24);
    noSatelliteLabel->setStyleSheet("color: rgb(235, 235, 235); font-weight: 600;");
    noSatelliteLabel->hide();

    // Clicking a satellite row is what wakes up the detail panel and the 3D selection.
    connect(ui->SatelliteTable, &QTableWidget::cellClicked, this, &mainwindow::OnSatelliteRowClicked);

    // Tiny clock goblin that keeps poking the UI once a second.
    refreshTimer = new QTimer(this);
    connect(refreshTimer , &QTimer::timeout ,this , &mainwindow::RefreshTelemetryView);

    RefreshTelemetryView();
    refreshTimer->start(1000);
}

mainwindow::~mainwindow()
{
    delete ui;
}

// Sets up the top table that lists the latest known state for each satellite.
void mainwindow::ConfigureSatelliteTable()
{
    ui->SatelliteTable->setColumnCount(4);
    ui->SatelliteTable->setHorizontalHeaderLabels({
        "Satellite Name",
        "Link Status",
        "Battery",
        "Temperature"
    });
    ui->SatelliteTable->setRowCount(0);
    ui->SatelliteTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->SatelliteTable->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->SatelliteTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->SatelliteTable->setAlternatingRowColors(true);
    ui->SatelliteTable->verticalHeader()->setVisible(false);
    ui->SatelliteTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}

// Sets up the lower telemetry history table for the currently selected satellite.
void mainwindow::ConfigureTelemetryTable()
{
    ui->DatabaseTable->setColumnCount(10);
    ui->DatabaseTable->setHorizontalHeaderLabels({
        "Sequence",
        "Timesent_ms",
        "Received_ms",
        "Latency_ms",
        "Age_ms",
        "Link Status",
        "Battery",
        "Temperature",
        "PacketSize",
        "Date & Time"
    });
    ui->DatabaseTable->setRowCount(0);
    ui->DatabaseTable->setSelectionMode(QAbstractItemView::NoSelection);
    ui->DatabaseTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->DatabaseTable->setAlternatingRowColors(true);
    ui->DatabaseTable->verticalHeader()->setVisible(false);
    ui->DatabaseTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    ui->DatabaseTable->horizontalHeader()->setDefaultSectionSize(120);
    ui->DatabaseTable->horizontalHeader()->setStretchLastSection(true);
    ui->DatabaseTable->setCursor(Qt::ArrowCursor);
}

// Gives the widgets a cleaner mission-control look without messing with the actual UI layout too much.
void mainwindow::StyleTheUi()
{
    // Just enough styling so the panels feel a bit more mission-control and a bit less default-widget-core.
    const QString tableStyle =
        "QTableWidget {"
        " background-color: rgba(8, 14, 24, 210);"
        " color: rgb(232, 238, 245);"
        " gridline-color: rgba(120, 150, 180, 70);"
        " border: 1px solid rgba(120, 150, 180, 90);"
        " border-radius: 10px;"
        " selection-background-color: rgba(72, 145, 220, 160);"
        " selection-color: white;"
        "}"
        "QHeaderView::section {"
        " background-color: rgba(20, 30, 46, 220);"
        " color: rgb(226, 234, 244);"
        " border: none;"
        " padding: 6px;"
        " font-weight: 600;"
        "}";

    ui->SatelliteTable->setStyleSheet(tableStyle);
    ui->DatabaseTable->setStyleSheet(tableStyle);

    ui->layoutWidget->setAttribute(Qt::WA_StyledBackground, true);
    ui->layoutWidget_2->setAttribute(Qt::WA_StyledBackground, true);

    const QString panelStyle =
        "QWidget {"
        " background-color: rgba(8, 14, 24, 170);"
        " border: 1px solid rgba(120, 150, 180, 90);"
        " border-radius: 12px;"
        "}"
        "QLabel {"
        " color: rgb(232, 238, 245);"
        " background: transparent;"
        " border: none;"
        "}";

    ui->layoutWidget->setStyleSheet(panelStyle);
    ui->layoutWidget_2->setStyleSheet(panelStyle);

    // These help keep the left and right info stacks visually centered inside their own little cards.
    ui->SatelliteData_Left->setContentsMargins(12, 12, 12, 12);
    ui->SatelliteData_Right->setContentsMargins(12, 12, 12, 12);
    ui->SatelliteData_Left->setSpacing(8);
    ui->SatelliteData_Right->setSpacing(12);
    ui->SatelliteData_Left->setAlignment(Qt::AlignCenter);
    ui->SatelliteData_Right->setAlignment(Qt::AlignCenter);

    // Center each row too so the text does not lean awkwardly to one side.
    ui->SatelliteName_Layout->setAlignment(Qt::AlignHCenter);
    ui->LinkStatus_Layout->setAlignment(Qt::AlignHCenter);
    ui->PacketLoss_Layout->setAlignment(Qt::AlignHCenter);
    ui->Latency_Layout->setAlignment(Qt::AlignHCenter);
    ui->Battery_Layout->setAlignment(Qt::AlignHCenter);
    ui->Temprature_Layout->setAlignment(Qt::AlignHCenter);
    ui->Battery_StatusLayout->setAlignment(Qt::AlignHCenter);
    ui->Health_StatusLayout->setAlignment(Qt::AlignHCenter);
    ui->Temp_StatusLayout->setAlignment(Qt::AlignHCenter);
}

// Fills the top satellite table using the newest telemetry row we have for each satellite.
void mainwindow::PopulateSatelliteTable()
{
    ui->SatelliteTable->setRowCount(satellites.size());

    for (int row = 0; row < satellites.size(); ++row)
    {
        const SatelliteRow& sat = satellites[row];

        QTableWidgetItem* nameItem = new QTableWidgetItem(sat.name);
        nameItem->setData(Qt::UserRole, sat.name);
        ui->SatelliteTable->setItem(row, 0, nameItem);

        ui->SatelliteTable->setItem(row, 1, new QTableWidgetItem(sat.linkStatus));
        ui->SatelliteTable->setItem(row, 2, new QTableWidgetItem(QString::number(sat.battery, 'f', 2) + " %"));
        ui->SatelliteTable->setItem(row, 3, new QTableWidgetItem(QString::number(sat.temperature, 'f', 2) + " C"));
    }
}

// Loads the recent packet history for whichever satellite the operator clicked on.
void mainwindow::PopulateTelemetryTable()
{
    ui->DatabaseTable->setRowCount(0);

    // No selected satellite means no detail history yet, so we quietly bail out.
    if (selectedSatelliteName.isEmpty())
    {
        return;
    }

    // Use the same DB path the refresh loop picked, otherwise we might read from the wrong random build folder copy.
    const QString dbpath = !activeDatabasePath.isEmpty() ? activeDatabasePath : ResolveDatabasePath();
    if (dbpath.isEmpty())
    {
        return;
    }

    sqlite3* DB = nullptr;
    sqlite3_stmt* stmt = nullptr;

    QByteArray dbPathUtf8 = dbpath.toUtf8();
    int rc = sqlite3_open_v2(dbPathUtf8.constData(), &DB, SQLITE_OPEN_READONLY, nullptr);
    if (rc != SQLITE_OK)
    {
        std::cerr << "Failed to open database ERROR  : " << sqlite3_errmsg(DB) << "\n";
        sqlite3_close(DB);
        return;
    }

    const char* sql =
        "SELECT Sequence, timestamp_ms, received_ms, battery, temperature "
        "FROM Telemetry "
        "WHERE Satellite_name = ? "
        "ORDER BY id DESC "
        "LIMIT 20";

    if (sqlite3_prepare_v2(DB, sql, -1, &stmt, nullptr) == SQLITE_OK)
    {
        // Grab a short packet trail so the operator can see more than one lonely row.
        QByteArray satNameUtf8 = selectedSatelliteName.toUtf8();
        sqlite3_bind_text(stmt, 1, satNameUtf8.constData(), -1, SQLITE_TRANSIENT);

        while (sqlite3_step(stmt) == SQLITE_ROW)
        {
            int row = ui->DatabaseTable->rowCount();
            ui->DatabaseTable->insertRow(row);

            qint64 sequence = sqlite3_column_int64(stmt, 0);
            qint64 timestampMs = sqlite3_column_int64(stmt, 1);
            qint64 receivedMs = sqlite3_column_int64(stmt, 2);
            double battery = sqlite3_column_double(stmt, 3);
            double temperature = sqlite3_column_double(stmt, 4);
            qint64 latencyMs = receivedMs - timestampMs;
            qint64 ageMs = NowMs() - receivedMs;
            QString linkStatus = GetLinkStatus(ageMs);
            QString packetSize = GuessPacketSize(selectedSatelliteName, sequence, timestampMs, battery, temperature);
            QString displayTime = FormatTimeText(receivedMs);

            ui->DatabaseTable->setItem(row, 0, new QTableWidgetItem(QString::number(sequence)));
            ui->DatabaseTable->setItem(row, 1, new QTableWidgetItem(QString::number(timestampMs)));
            ui->DatabaseTable->setItem(row, 2, new QTableWidgetItem(QString::number(receivedMs)));
            ui->DatabaseTable->setItem(row, 3, new QTableWidgetItem(QString::number(latencyMs)));
            ui->DatabaseTable->setItem(row, 4, new QTableWidgetItem(QString::number(ageMs)));
            ui->DatabaseTable->setItem(row, 5, new QTableWidgetItem(linkStatus));
            ui->DatabaseTable->setItem(row, 6, new QTableWidgetItem(QString::number(battery, 'f', 2) + " %"));
            ui->DatabaseTable->setItem(row, 7, new QTableWidgetItem(QString::number(temperature, 'f', 2) + " C"));
            ui->DatabaseTable->setItem(row, 8, new QTableWidgetItem(packetSize));
            ui->DatabaseTable->setItem(row, 9, new QTableWidgetItem(displayTime));
        }
    }
    else
    {
        std::cerr << "Prepare has failed , Error in file mainwindow.cpp \n";
    }

    sqlite3_finalize(stmt);
    sqlite3_close(DB);
}

// Pushes the clicked satellite into the labels, telemetry table, and OrbitView.
void mainwindow::ApplySelectedSatellite()
{
    if (selectedSatelliteName.isEmpty())
    {
        ShowNoSelectionState();
        return;
    }

    // Walk the cached list until we find the one the user asked for.
    for (int row = 0; row < satellites.size(); ++row)
    {
        const SatelliteRow& sat = satellites[row];
        if (sat.name != selectedSatelliteName)
        {
            continue;
        }

        QString batteryStatus = GetBatteryStatus(sat.battery);
        QString temperatureStatus = GetTempStatus(sat.temperature);
        QString healthStatus = GetHealthStatus(sat.linkStatus, batteryStatus, temperatureStatus);
        qint64 latency = sat.receivedMs - sat.timestampMs;

        ui->SatelliteName_Label->setText(sat.name);
        ui->LinkStatus_Connection->setText(sat.linkStatus);
        ui->PacketLoss_Connection->setText("N/A");
        ui->Latency_Data->setText(QString::number(latency) + " ms");
        ui->Battery_Data->setText(QString::number(sat.battery, 'f', 2));
        ui->Temprature_Data->setText(QString::number(sat.temperature, 'f', 2));
        ui->Battery_StatusData->setText(batteryStatus);
        ui->Temp_StatusData->setText(temperatureStatus);
        ui->Health_StatusData->setText(healthStatus);

        ShowTelemetryState();
        PopulateTelemetryTable();

        if (ui->OrbitView != nullptr)
        {
            ui->OrbitView->SetSelectedSatellite(sat.name);
            ui->OrbitView->SetSatelliteLinkStatus(sat.linkStatus);
        }

        return;
    }

    selectedSatelliteName.clear();
    ShowNoSelectionState();
}

// This is the "backend is asleep or empty" state where we hide the juicy telemetry stuff.
void mainwindow::ShowNoTelemetryState()
{
    ui->SatelliteTable->setEnabled(false);
    ui->SatelliteTable->clearSelection();
    ui->SatelliteTable->setRowCount(0);
    ui->DatabaseTable->setEnabled(false);
    ui->DatabaseTable->clearSelection();
    ui->DatabaseTable->setRowCount(0);
    ui->DatabaseTable->setVisible(false);
    ui->layoutWidget->setVisible(false);
    ui->layoutWidget_2->setVisible(false);

    ClearDetailLabels(ui);

    if (noSatelliteLabel != nullptr)
    {
        noSatelliteLabel->setText("No active satellite connection");
        noSatelliteLabel->show();
        noSatelliteLabel->raise();
    }

    if (ui->OrbitView != nullptr)
    {
        ui->OrbitView->SetSelectedSatellite(QString());
        ui->OrbitView->SetSatelliteLinkStatus("Disconnected");
    }

    activeDatabasePath.clear();
}

// This is the middle state: telemetry exists, but the user has not picked a satellite yet.
void mainwindow::ShowNoSelectionState()
{
    ui->SatelliteTable->setEnabled(true);
    ui->SatelliteTable->clearSelection();
    ui->DatabaseTable->setEnabled(false);
    ui->DatabaseTable->setRowCount(0);
    ui->DatabaseTable->setVisible(false);
    ui->layoutWidget->setVisible(false);
    ui->layoutWidget_2->setVisible(false);

    ClearDetailLabels(ui);

    if (noSatelliteLabel != nullptr)
    {
        noSatelliteLabel->setText("Select a satellite to view telemetry");
        noSatelliteLabel->show();
        noSatelliteLabel->raise();
    }

    if (ui->OrbitView != nullptr)
    {
        ui->OrbitView->SetSelectedSatellite(QString());
        ui->OrbitView->SetSatelliteLinkStatus("Disconnected");
    }
}

// Makes the detail widgets visible again once a real satellite selection happens.
void mainwindow::ShowTelemetryState()
{
    ui->SatelliteTable->setEnabled(true);
    ui->DatabaseTable->setEnabled(true);
    ui->DatabaseTable->setVisible(true);
    ui->layoutWidget->setVisible(true);
    ui->layoutWidget_2->setVisible(true);

    if (noSatelliteLabel != nullptr)
    {
        noSatelliteLabel->hide();
    }
}

// Row click handler for the satellite list. Nice and tiny on purpose.
void mainwindow::OnSatelliteRowClicked(int row, int column)
{
    Q_UNUSED(column)

    if (row < 0 || row >= satellites.size())
    {
        return;
    }

    selectedSatelliteName = satellites[row].name;
    ApplySelectedSatellite();
}

// This is the heartbeat of the window. Every tick we re-read the newest per-satellite state from SQLite.
void mainwindow::RefreshTelemetryView()
{
    // Start fresh each pass so the tables always reflect what is actually in the DB right now.
    satellites.clear();
    ui->SatelliteTable->setRowCount(0);
    ui->DatabaseTable->setRowCount(0);

    // Figure out which Soul.db is the live one, because Qt build folders love making clones everywhere.
    activeDatabasePath = ResolveDatabasePath();
    const QString dbpath = activeDatabasePath;
    if (dbpath.isEmpty())
    {
        selectedSatelliteName.clear();
        ShowNoTelemetryState();
        return;
    }

    sqlite3* DB = nullptr;
    sqlite3_stmt* stmt = nullptr;

    QByteArray dbPathUtf8 = dbpath.toUtf8();
    int rc = sqlite3_open_v2(dbPathUtf8.constData(), &DB, SQLITE_OPEN_READONLY, nullptr);
    if (rc != SQLITE_OK)
    {
        std::cerr << "Failed to open database ERROR  : " << sqlite3_errmsg(DB) << "\n";
        sqlite3_close(DB);
        selectedSatelliteName.clear();
        ShowNoTelemetryState();
        return;
    }

    const char* sql =
        "SELECT t.Satellite_name, t.sequence, t.battery, t.temperature, t.received_ms, t.timestamp_ms "
        "FROM Telemetry t "
        "WHERE t.id = ("
        "    SELECT MAX(t2.id) "
        "    FROM Telemetry t2 "
        "    WHERE t2.Satellite_name = t.Satellite_name"
        ") "
        "ORDER BY t.id DESC";

    if (sqlite3_prepare_v2(DB, sql, -1, &stmt, nullptr) == SQLITE_OK)
    {
        qint64 currentTime = NowMs();

        // Each row here is already "latest row for a satellite", so this becomes the top selection list.
        while (sqlite3_step(stmt) == SQLITE_ROW)
        {
            const unsigned char* satName = sqlite3_column_text(stmt, 0);
            if (satName == nullptr)
            {
                continue;
            }

            SatelliteRow sat;
            sat.name = QString::fromUtf8(reinterpret_cast<const char*>(satName));
            sat.sequence = sqlite3_column_int64(stmt, 1);
            sat.battery = sqlite3_column_double(stmt, 2);
            sat.temperature = sqlite3_column_double(stmt, 3);
            sat.receivedMs = sqlite3_column_int64(stmt, 4);
            sat.timestampMs = sqlite3_column_int64(stmt, 5);
            sat.linkStatus = GetLinkStatus(currentTime - sat.receivedMs);

            satellites.push_back(sat);
        }
    }
    else
    {
        std::cerr << "Prepare has failed , Error in file mainwindow.cpp \n";
    }

    sqlite3_finalize(stmt);
    sqlite3_close(DB);

    if (satellites.isEmpty())
    {
        selectedSatelliteName.clear();
        ShowNoTelemetryState();
        return;
    }

    PopulateSatelliteTable();

    // Try to preserve the current selection if that satellite still exists in the fresh query result.
    int selectedRow = -1;
    for (int row = 0; row < satellites.size(); ++row)
    {
        if (satellites[row].name == selectedSatelliteName)
        {
            selectedRow = row;
            break;
        }
    }

    if (selectedSatelliteName.isEmpty())
    {
        ShowNoSelectionState();
        return;
    }

    if (selectedRow < 0)
    {
        selectedSatelliteName.clear();
        ShowNoSelectionState();
        return;
    }

    {
        QSignalBlocker blocker(ui->SatelliteTable);
        ui->SatelliteTable->selectRow(selectedRow);
    }

    ApplySelectedSatellite();
}

// Hunts down the real Soul.db file instead of blindly trusting one build folder copy.
QString mainwindow::ResolveDatabasePath() const
{
    const QString appDirDb = QDir(QCoreApplication::applicationDirPath()).filePath("Soul.db");
    const QString currentDirDb = QDir::current().filePath("Soul.db");
    const QString repoRootDb = "C:/SOUL/Soul.db";
    const QString buildRootDb = "C:/SOUL/build/Soul.db";

    const QStringList candidates = {
        activeDatabasePath,
        appDirDb,
        currentDirDb,
        repoRootDb,
        buildRootDb,
        "C:/SOUL/build/Debug/Soul.db",
        "C:/SOUL/build/Desktop_Qt_6_11_0_llvm_mingw_64_bit-Debug/Soul.db",
        "C:/SOUL/cmake-build-debug/Soul.db"
    };

    QString bestPathWithRows;
    QString bestEmptyPath;
    QDateTime bestWithRowsTime;
    QDateTime bestEmptyTime;

    // We check a few likely places, then prefer the newest DB that actually has telemetry rows in it.
    for (const QString& path : candidates)
    {
        QFileInfo info(path);
        if (!info.exists() || info.size() == 0)
        {
            continue;
        }

        sqlite3* DB = nullptr;
        sqlite3_stmt* stmt = nullptr;
        QByteArray dbPathUtf8 = path.toUtf8();

        if (sqlite3_open_v2(dbPathUtf8.constData(), &DB, SQLITE_OPEN_READONLY, nullptr) != SQLITE_OK)
        {
            sqlite3_close(DB);
            continue;
        }

        bool hasTelemetryTable = false;
        int telemetryRowCount = 0;

        const char* hasTableSql =
            "SELECT 1 "
            "FROM sqlite_master "
            "WHERE type = 'table' AND name = 'Telemetry' "
            "LIMIT 1";

        if (sqlite3_prepare_v2(DB, hasTableSql, -1, &stmt, nullptr) == SQLITE_OK)
        {
            hasTelemetryTable = (sqlite3_step(stmt) == SQLITE_ROW);
        }

        sqlite3_finalize(stmt);
        stmt = nullptr;

        if (hasTelemetryTable)
        {
            // We want the DB that is actually alive, not the empty decoy one hiding in another build folder.
            const char* countSql = "SELECT COUNT(*) FROM Telemetry";
            if (sqlite3_prepare_v2(DB, countSql, -1, &stmt, nullptr) == SQLITE_OK && sqlite3_step(stmt) == SQLITE_ROW)
            {
                telemetryRowCount = sqlite3_column_int(stmt, 0);
            }
        }

        sqlite3_finalize(stmt);
        sqlite3_close(DB);

        if (!hasTelemetryTable)
        {
            continue;
        }

        if (telemetryRowCount > 0)
        {
            if (bestPathWithRows.isEmpty() || info.lastModified() > bestWithRowsTime)
            {
                bestPathWithRows = path;
                bestWithRowsTime = info.lastModified();
            }
        }
        else if (bestEmptyPath.isEmpty() || info.lastModified() > bestEmptyTime)
        {
            bestEmptyPath = path;
            bestEmptyTime = info.lastModified();
        }
    }

    if (!bestPathWithRows.isEmpty())
    {
        return bestPathWithRows;
    }

    return bestEmptyPath;
}

// Placeholder for later if database setup gets moved out of the window class.
const char * mainwindow::LoadDatabase()
{
    //FIXME  - Separate database logic later here //

    return nullptr;
}
