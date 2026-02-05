#pragma once

#include <QMainWindow>   
#include "BacModel.h"

class QLineEdit;
class QComboBox;
class QButtonGroup;
class QRadioButton;
class QDateTimeEdit;
class QStackedWidget;
class QPushButton;
class QSpinBox;
class QTableWidget;
class QLabel;
class QCheckBox;

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);

private slots:
    void onCalculate();
    void onAddDrink();
    void onRemoveDrink();
    void onEditDrink();
    void onModeSimplified();
    void onModeDetailed();
    void onDrinkTypeChanged();
    void onDrinkSizeChanged();
    void onToggleTheme();

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

    QSpinBox* simpleDrinkCount;

    QComboBox* drinkTypeCombo;
    QComboBox* drinkSizeCombo;
    QSpinBox* drinkCountSpin;
    QLineEdit* drinkVolumeEdit;
    QLineEdit* drinkAbvEdit;
    QPushButton* addDrinkBtn;
    QPushButton* removeDrinkBtn;
    QPushButton* editDrinkBtn;
    QTableWidget* drinksTable;

    QLabel* resultBac;
    QLabel* resultDrive;
    QLabel* resultSober;
    QLabel* resultSoberTime;
    QLabel* resultTargetStatus;
    QLabel* resultTargetDrive;
    QLabel* resultMaxBeers;
    QLabel* plotLabel;

    QCheckBox* targetSoberCheck;
    QDateTimeEdit* targetSoberEdit;

    QPushButton* themeBtn;
    bool darkMode = false;

    void setupUi();
    void updateDrinkDefaults();
    void applyTheme();
    double totalAlcoholGramsFromTable() const;
    int calculateMaxBeers(double weightKg, Gender gender, double hoursUntilTarget) const;
};
