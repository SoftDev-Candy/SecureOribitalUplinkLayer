#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>
#include <QVector>
#include <QTimer>
#include <QString>

class QLabel;

namespace Ui {
class mainwindow;
}

class mainwindow : public QWidget
{
    Q_OBJECT

public:
    explicit mainwindow(QWidget *parent = nullptr );
    ~mainwindow();

    // Reads the latest telemetry rows and refreshes the operator widgets from the database.
    void RefreshTelemetryView();
    const char* LoadDatabase();

private:
    struct SatelliteRow
    {
        QString name;
        QString linkStatus;
        qint64 sequence = 0;
        qint64 receivedMs = 0;
        qint64 timestampMs = 0;
        double battery = 0.0;
        double temperature = 0.0;
    };

    // Sets up the satellite table so it behaves like a selection list instead of a raw dump.
    void ConfigureSatelliteTable();
    // Sets up the telemetry table so it shows recent packets for the chosen satellite.
    void ConfigureTelemetryTable();
    // Copies the latest row for each satellite into the visible list.
    void PopulateSatelliteTable();
    // Copies recent telemetry rows for the selected satellite into the lower table.
    void PopulateTelemetryTable();
    // Pushes the currently selected satellite values into the detailed labels and Orbitview.
    void ApplySelectedSatellite();
    // Switches the operator UI into the "nothing is alive right now" state.
    void ShowNoTelemetryState();
    // Hides the detailed telemetry widgets until the operator has a satellite picked.
    void ShowNoSelectionState();
    // Brings the detailed widgets back once telemetry exists again.
    void ShowTelemetryState();
    // Handles row clicks in the satellite table and remembers which satellite the operator chose.
    void OnSatelliteRowClicked(int row, int column);
    // Finds the database file that actually has telemetry instead of the empty build goblin copy.
    QString ResolveDatabasePath() const;

    Ui::mainwindow *ui;
    QTimer* refreshTimer = nullptr;
    QVector<SatelliteRow> satellites;
    QString selectedSatelliteName;
    QString activeDatabasePath;
    QLabel* noSatelliteLabel = nullptr;

};

#endif // MAINWINDOW_H
