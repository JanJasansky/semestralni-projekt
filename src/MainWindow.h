#pragma once
#include <QMainWindow>
#include <QLabel>
#include <QComboBox>
#include <QLineEdit>
#include <QPushButton>
#include <QRadioButton>
#include <QButtonGroup>
#include <QDateTimeEdit>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QTableWidget>
#include <QStackedWidget>
#include <QSpinBox>

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);

private slots:
    void onCalculate();
    void onAddDrink();
    void onModeSimplified();
    void onModeDetailed();
    void onDrinkTypeChanged();
    void onDrinkSizeChanged();

private:
    QLineEdit* weightEdit;
    QComboBox* weightUnit;
    QButtonGroup* genderGroup;
    QRadioButton* maleBtn;
    QRadioButton* femaleBtn;
    QDateTimeEdit* startTimeEdit;
    QDateTimeEdit* endTimeEdit;
    QStackedWidget* modeStack;
    QPushButton* simplifiedBtn;
    QPushButton* detailedBtn;

    // Simplified mode
    QSpinBox* simpleDrinkCount;

    // Detailed mode
    QComboBox* drinkTypeCombo;
    QComboBox* drinkSizeCombo;
    QSpinBox* drinkCountSpin;
    QLineEdit* drinkVolumeEdit;
    QLineEdit* drinkAbvEdit;
    QPushButton* addDrinkBtn;
    QTableWidget* drinksTable;

    QLabel* resultBac;
    QLabel* resultDrive;
    QLabel* resultSober;
    QLabel* plotLabel;

    void setupUi();
    void updateDrinkDefaults();
    double totalAlcoholGramsFromTable() const;
};
