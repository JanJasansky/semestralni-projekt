#include "MainWindow.h"

#include <QWidget>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
#include <QDateTimeEdit>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QStackedWidget>
#include <QSpinBox>
#include <QRadioButton>
#include <QButtonGroup>
#include "PythonPlotter.h"
#include <Qt>        
#include <QDateTime> 
#include <QLabel>
#include <QPixmap>
#include <QHeaderView>
#include <QFrame>
#include <QAbstractItemView>
#include <QDoubleValidator>
#include <QMessageBox>
#include <QFile>
#include <QTextStream>
#include <QCheckBox>
#include <QInputDialog>
#include <QApplication>

static DrinkProfile defaultProfile(const QString& type, const QString& size) {
    if (type == "Pivo") {
        if (size == "Malé (0,3 l)") return {300.0, 12.0};
        return {500.0, 12.0}; // Velké (0,5 l)
    }
    if (type == "Víno") {
        if (size == "Sklenička (0,2 l)") return {200.0, 12.0};
        return {100.0, 12.0}; // Degustace (0,1 l)
    }
    if (type == "Panák") {
        if (size == "Malý (0,02 l)") return {20.0, 40.0};
        return {40.0, 40.0}; // Velký (0,04 l)
    }
    if (type == "Koktejl") {
        if (size == "Malý (0,2 l)") return {200.0, 20.0};
        return {300.0, 20.0}; // Velký (0,3 l)
    }
    return {100.0, 10.0}; // Vlastní (default)
}

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    setupUi();
    applyTheme();
}

void MainWindow::setupUi() {
    auto* central = new QWidget;
    auto* root = new QVBoxLayout;

    // Title + mode buttons
    auto* header = new QHBoxLayout;
    auto* titleBox = new QVBoxLayout;
    auto* title = new QLabel("Alkoholická kalkulačka");
    title->setStyleSheet("font-size: 26px; font-weight: 600;");
    auto* subtitle = new QLabel("Jednoduchý odhad promile a průběhu v čase");
    subtitle->setStyleSheet("color: #6a625b;");
    titleBox->addWidget(title);
    titleBox->addWidget(subtitle);
    header->addLayout(titleBox);
    header->addStretch();

    simplifiedBtn = new QPushButton("Zjednodušený režim");
    detailedBtn = new QPushButton("Podrobný režim");
    simplifiedBtn->setCheckable(true);
    detailedBtn->setCheckable(true);
    simplifiedBtn->setChecked(true);
    simplifiedBtn->setStyleSheet("padding: 6px 16px; border-radius: 16px; background: #f4d2c0;");
    detailedBtn->setStyleSheet("padding: 6px 16px; border-radius: 16px; background: #f6efe8;");
    connect(simplifiedBtn, &QPushButton::clicked, this, &MainWindow::onModeSimplified);
    connect(detailedBtn, &QPushButton::clicked, this, &MainWindow::onModeDetailed);

    themeBtn = new QPushButton("Tmavý režim");
    connect(themeBtn, &QPushButton::clicked, this, &MainWindow::onToggleTheme);

    header->addWidget(simplifiedBtn);
    header->addWidget(detailedBtn);
    header->addWidget(themeBtn);
    root->addLayout(header);

    auto* line = new QFrame;
    line->setFrameShape(QFrame::HLine);
    line->setStyleSheet("color: #e1d6cc;");
    root->addWidget(line);

    // Main columns
    auto* columns = new QHBoxLayout;

    // Základní údaje
    auto* basicsBox = new QGroupBox("Základní údaje");
    auto* basicsLayout = new QVBoxLayout;
    basicsLayout->setSpacing(2);
    basicsLayout->setContentsMargins(8, 8, 8, 8);

    auto* weightLabel = new QLabel("Hmotnost");
    auto* weightRow = new QHBoxLayout;
    weightRow->setSpacing(4);
    weightEdit = new QLineEdit;
    weightEdit->setValidator(new QDoubleValidator(0, 500, 2, this));
    weightUnit = new QComboBox;
    weightUnit->addItems({"kg", "lb"});
    weightRow->addWidget(weightEdit);
    weightRow->addWidget(weightUnit);
    basicsLayout->addWidget(weightLabel);
    basicsLayout->addLayout(weightRow);
    basicsLayout->addSpacing(6);

    auto* genderLabel = new QLabel("Pohlaví");
    auto* genderRow = new QHBoxLayout;
    genderRow->setSpacing(4);
    maleBtn = new QRadioButton("Muž");
    femaleBtn = new QRadioButton("Žena");
    maleBtn->setChecked(true);
    genderGroup = new QButtonGroup(this);
    genderGroup->addButton(maleBtn);
    genderGroup->addButton(femaleBtn);
    genderRow->addWidget(maleBtn);
    genderRow->addWidget(femaleBtn);
    basicsLayout->addWidget(genderLabel);
    basicsLayout->addLayout(genderRow);
    basicsLayout->addSpacing(6);

    auto* startLabel = new QLabel("Začátek pití");
    startTimeEdit = new QDateTimeEdit(QDateTime::currentDateTime());
    startTimeEdit->setDisplayFormat("dd.MM.yyyy HH:mm");
    startTimeEdit->setCalendarPopup(true);
    basicsLayout->addWidget(startLabel);
    basicsLayout->addWidget(startTimeEdit);
    basicsLayout->addSpacing(6);

    auto* endLabel = new QLabel("Konec pití");
    endTimeEdit = new QDateTimeEdit(QDateTime::currentDateTime());
    endTimeEdit->setDisplayFormat("dd.MM.yyyy HH:mm");
    endTimeEdit->setCalendarPopup(true);
    basicsLayout->addWidget(endLabel);
    basicsLayout->addWidget(endTimeEdit);
    basicsLayout->addSpacing(6);

    targetSoberCheck = new QCheckBox("Cílový čas střízlivosti");
    targetSoberEdit = new QDateTimeEdit(QDateTime::currentDateTime().addSecs(8 * 3600));
    targetSoberEdit->setDisplayFormat("dd.MM.yyyy HH:mm");
    targetSoberEdit->setCalendarPopup(true);
    targetSoberEdit->setEnabled(false);
    connect(targetSoberCheck, &QCheckBox::toggled, targetSoberEdit, &QDateTimeEdit::setEnabled);

    basicsLayout->addWidget(targetSoberCheck);
    basicsLayout->addWidget(targetSoberEdit);

    basicsBox->setLayout(basicsLayout);
    columns->addWidget(basicsBox, 1);

    // Nápoje
    auto* drinksBox = new QGroupBox("Nápoje");
    auto* drinksLayout = new QVBoxLayout;

    modeStack = new QStackedWidget;

    // Simplified mode widget
    auto* simpleWidget = new QWidget;
    auto* simpleLayout = new QVBoxLayout;
    auto* simpleLabel = new QLabel("Počet drinků (standardní dávky)");
    simpleDrinkCount = new QSpinBox;
    simpleDrinkCount->setRange(0, 50);
    auto* simpleHint = new QLabel("Zjednodušený odhad: 1 drink ≈ 0,5 l piva nebo 0,2 l vína nebo 0,04 l panáku.");
    simpleHint->setWordWrap(true);
    simpleLayout->addWidget(simpleLabel);
    simpleLayout->addWidget(simpleDrinkCount);
    simpleLayout->addWidget(simpleHint);
    simpleWidget->setLayout(simpleLayout);

    // Detailed mode widget
    auto* detailedWidget = new QWidget;
    auto* detailedLayout = new QVBoxLayout;

    auto* formRow = new QGridLayout;
    formRow->addWidget(new QLabel("Typ"), 0, 0);
    formRow->addWidget(new QLabel("Velikost"), 0, 1);
    formRow->addWidget(new QLabel("Počet"), 0, 2);
    formRow->addWidget(new QLabel("Objem (ml)"), 0, 3);
    formRow->addWidget(new QLabel("ABV (%)"), 0, 4);

    drinkTypeCombo = new QComboBox;
    drinkTypeCombo->addItems({"Pivo", "Víno", "Panák", "Koktejl", "Vlastní"});
    drinkSizeCombo = new QComboBox;
    drinkCountSpin = new QSpinBox;
    drinkCountSpin->setRange(1, 20);
    drinkVolumeEdit = new QLineEdit;
    drinkAbvEdit = new QLineEdit;
    drinkVolumeEdit->setValidator(new QDoubleValidator(0, 2000, 2, this));
    drinkAbvEdit->setValidator(new QDoubleValidator(0, 100, 2, this));

    formRow->addWidget(drinkTypeCombo, 1, 0);
    formRow->addWidget(drinkSizeCombo, 1, 1);
    formRow->addWidget(drinkCountSpin, 1, 2);
    formRow->addWidget(drinkVolumeEdit, 1, 3);
    formRow->addWidget(drinkAbvEdit, 1, 4);

    addDrinkBtn = new QPushButton("Přidat");
    editDrinkBtn = new QPushButton("Upravit");
    removeDrinkBtn = new QPushButton("Odebrat");
    formRow->addWidget(addDrinkBtn, 1, 5);
    formRow->addWidget(editDrinkBtn, 1, 6);
    formRow->addWidget(removeDrinkBtn, 1, 7);

    detailedLayout->addLayout(formRow);

    drinksTable = new QTableWidget(0, 5);
    drinksTable->setHorizontalHeaderLabels({"Typ", "Velikost", "Počet", "Objem (ml)", "ABV (%)"});
    drinksTable->horizontalHeader()->setStretchLastSection(true);
    drinksTable->verticalHeader()->setVisible(false);
    drinksTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    drinksTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    detailedLayout->addWidget(drinksTable);

    detailedWidget->setLayout(detailedLayout);

    modeStack->addWidget(simpleWidget);
    modeStack->addWidget(detailedWidget);
    modeStack->setCurrentIndex(0);
    drinksLayout->addWidget(modeStack);

    drinksBox->setLayout(drinksLayout);
    columns->addWidget(drinksBox, 1);

    // Výsledky
    auto* resultsBox = new QGroupBox("Výsledky");
    auto* resultsLayout = new QVBoxLayout;
    resultBac = new QLabel("Aktuální odhad: 0,00 ‰");
    resultBac->setStyleSheet("font-size: 22px; font-weight: 600;");
    resultDrive = new QLabel("Řízení: neznámé");
    resultSober = new QLabel("Do vystřízlivění: -");
    resultSoberTime = new QLabel("Střízlivý v: -");
    resultTargetStatus = new QLabel("");
    resultTargetDrive = new QLabel("");
    resultMaxBeers = new QLabel("");
    auto* calcBtn = new QPushButton("Spočítat");
    connect(calcBtn, &QPushButton::clicked, this, &MainWindow::onCalculate);

    resultsLayout->addWidget(resultBac);
    resultsLayout->addWidget(resultDrive);
    resultsLayout->addWidget(resultSober);
    resultsLayout->addWidget(resultSoberTime);
    resultsLayout->addWidget(resultTargetStatus);
    resultsLayout->addWidget(resultTargetDrive);
    resultsLayout->addWidget(resultMaxBeers);
    resultsLayout->addSpacing(8);
    resultsLayout->addWidget(calcBtn);
    resultsLayout->addSpacing(8);

    auto* plotTitleRow = new QHBoxLayout;
    plotTitleRow->addWidget(new QLabel("Promile v čase"));
    plotTitleRow->addStretch();
    resultsLayout->addLayout(plotTitleRow);

    plotLabel = new QLabel;
    plotLabel->setMinimumHeight(240);
    plotLabel->setStyleSheet("background: #f6efe8; border: 1px solid #eadfd4;");
    plotLabel->setAlignment(Qt::AlignCenter);
    plotLabel->setText("Graf se zobrazí po výpočtu.");
    resultsLayout->addWidget(plotLabel);

    resultsBox->setLayout(resultsLayout);
    columns->addWidget(resultsBox, 1);

    root->addLayout(columns);
    central->setLayout(root);
    setCentralWidget(central);
    setWindowTitle("Alkoholická kalkulačka");

    // Signals
    connect(addDrinkBtn, &QPushButton::clicked, this, &MainWindow::onAddDrink);
    connect(editDrinkBtn, &QPushButton::clicked, this, &MainWindow::onEditDrink);
    connect(removeDrinkBtn, &QPushButton::clicked, this, &MainWindow::onRemoveDrink);
    connect(drinkTypeCombo, &QComboBox::currentTextChanged, this, &MainWindow::onDrinkTypeChanged);
    connect(drinkSizeCombo, &QComboBox::currentTextChanged, this, &MainWindow::onDrinkSizeChanged);

    updateDrinkDefaults();
}

void MainWindow::onModeSimplified() {
    simplifiedBtn->setChecked(true);
    detailedBtn->setChecked(false);
    modeStack->setCurrentIndex(0);
    if (darkMode) {
        simplifiedBtn->setStyleSheet("padding: 6px 16px; border-radius: 16px; background: #c4956a; color: #1e1e1e;");
        detailedBtn->setStyleSheet("padding: 6px 16px; border-radius: 16px; background: #4a4a4a; color: #ffffff;");
    } else {
        simplifiedBtn->setStyleSheet("padding: 6px 16px; border-radius: 16px; background: #f4d2c0;");
        detailedBtn->setStyleSheet("padding: 6px 16px; border-radius: 16px; background: #f6efe8;");
    }
}

void MainWindow::onModeDetailed() {
    simplifiedBtn->setChecked(false);
    detailedBtn->setChecked(true);
    modeStack->setCurrentIndex(1);
    if (darkMode) {
        simplifiedBtn->setStyleSheet("padding: 6px 16px; border-radius: 16px; background: #4a4a4a; color: #ffffff;");
        detailedBtn->setStyleSheet("padding: 6px 16px; border-radius: 16px; background: #c4956a; color: #1e1e1e;");
    } else {
        simplifiedBtn->setStyleSheet("padding: 6px 16px; border-radius: 16px; background: #f6efe8;");
        detailedBtn->setStyleSheet("padding: 6px 16px; border-radius: 16px; background: #f4d2c0;");
    }
}

void MainWindow::updateDrinkDefaults() {
    const QString type = drinkTypeCombo->currentText();

    drinkSizeCombo->blockSignals(true);
    drinkSizeCombo->clear();
    if (type == "Pivo") {
        drinkSizeCombo->addItems({"Malé (0,3 l)", "Velké (0,5 l)"});
    } else if (type == "Víno") {
        drinkSizeCombo->addItems({"Sklenička (0,2 l)", "Degustace (0,1 l)"});
    } else if (type == "Panák") {
        drinkSizeCombo->addItems({"Malý (0,02 l)", "Velký (0,04 l)"});
    } else if (type == "Koktejl") {
        drinkSizeCombo->addItems({"Malý (0,2 l)", "Velký (0,3 l)"});
    } else {
        drinkSizeCombo->addItems({"Vlastní"});
    }
    drinkSizeCombo->blockSignals(false);

    const QString size = drinkSizeCombo->currentText();
    const auto profile = defaultProfile(type, size);
    drinkVolumeEdit->setText(QString::number(profile.volume, 'f', 0));
    drinkAbvEdit->setText(QString::number(profile.abv, 'f', 1));

    const bool isCustom = (type == "Vlastní");
    drinkVolumeEdit->setReadOnly(!isCustom);
    drinkAbvEdit->setReadOnly(!isCustom);
}

void MainWindow::onDrinkTypeChanged() {
    updateDrinkDefaults();
}

void MainWindow::onDrinkSizeChanged() {
    updateDrinkDefaults();
}

void MainWindow::onAddDrink() {
    const QString type = drinkTypeCombo->currentText();
    const QString size = drinkSizeCombo->currentText();
    const int count = drinkCountSpin->value();
    const double volume = drinkVolumeEdit->text().toDouble();
    const double abv = drinkAbvEdit->text().toDouble();

    if (volume <= 0 || abv <= 0) {
        QMessageBox::warning(this, "Chybné údaje", "Zadej platný objem a procenta alkoholu.");
        return;
    }

    int row = drinksTable->rowCount();
    drinksTable->insertRow(row);
    drinksTable->setItem(row, 0, new QTableWidgetItem(type));
    drinksTable->setItem(row, 1, new QTableWidgetItem(size));
    drinksTable->setItem(row, 2, new QTableWidgetItem(QString::number(count)));
    drinksTable->setItem(row, 3, new QTableWidgetItem(QString::number(volume, 'f', 0)));
    drinksTable->setItem(row, 4, new QTableWidgetItem(QString::number(abv, 'f', 1)));
}

void MainWindow::onRemoveDrink() {
    int currentRow = drinksTable->currentRow();
    if (currentRow >= 0) {
        drinksTable->removeRow(currentRow);
    }
}

void MainWindow::onEditDrink() {
    int currentRow = drinksTable->currentRow();
    if (currentRow < 0) {
        QMessageBox::warning(this, "Chyba", "Vyber nápoj k úpravě.");
        return;
    }

    bool ok;
    int newCount = QInputDialog::getInt(this, "Upravit počet", "Počet:",
        drinksTable->item(currentRow, 2)->text().toInt(), 1, 100, 1, &ok);
    if (ok) {
        drinksTable->item(currentRow, 2)->setText(QString::number(newCount));
    }

    double newVolume = QInputDialog::getDouble(this, "Upravit objem", "Objem (ml):",
        drinksTable->item(currentRow, 3)->text().toDouble(), 1, 5000, 0, &ok);
    if (ok) {
        drinksTable->item(currentRow, 3)->setText(QString::number(newVolume, 'f', 0));
    }

    double newAbv = QInputDialog::getDouble(this, "Upravit ABV", "ABV (%):",
        drinksTable->item(currentRow, 4)->text().toDouble(), 0, 100, 1, &ok);
    if (ok) {
        drinksTable->item(currentRow, 4)->setText(QString::number(newAbv, 'f', 1));
    }
}

void MainWindow::onToggleTheme() {
    darkMode = !darkMode;
    applyTheme();
}

void MainWindow::applyTheme() {
    if (darkMode) {
        themeBtn->setText("Světlý režim");
        qApp->setStyleSheet(R"(
            QMainWindow, QWidget { background-color: #1e1e1e; color: #e0e0e0; }
            QGroupBox { border: 1px solid #444; border-radius: 4px; margin-top: 8px; padding-top: 8px; }
            QGroupBox::title { color: #e0e0e0; }
            QLineEdit, QComboBox, QSpinBox, QDateTimeEdit { background-color: #2d2d2d; border: 1px solid #444; color: #e0e0e0; padding: 4px; }
            QPushButton { background-color: #4a4a4a; border: 1px solid #666; color: #ffffff; padding: 6px 12px; border-radius: 4px; }
            QPushButton:hover { background-color: #5a5a5a; }
            QPushButton:checked { background-color: #c4956a; color: #1e1e1e; }
            QTableWidget { background-color: #2d2d2d; color: #e0e0e0; gridline-color: #444; }
            QHeaderView::section { background-color: #3d3d3d; color: #e0e0e0; border: 1px solid #444; }
            QLabel { color: #e0e0e0; }
            QRadioButton, QCheckBox { color: #e0e0e0; }
        )");
        plotLabel->setStyleSheet("background: #2d2d2d; border: 1px solid #444;");
        simplifiedBtn->setStyleSheet(simplifiedBtn->isChecked()
            ? "padding: 6px 16px; border-radius: 16px; background: #c4956a; color: #1e1e1e;"
            : "padding: 6px 16px; border-radius: 16px; background: #4a4a4a; color: #ffffff;");
        detailedBtn->setStyleSheet(detailedBtn->isChecked()
            ? "padding: 6px 16px; border-radius: 16px; background: #c4956a; color: #1e1e1e;"
            : "padding: 6px 16px; border-radius: 16px; background: #4a4a4a; color: #ffffff;");
    } else {
        themeBtn->setText("Tmavý režim");
        qApp->setStyleSheet("");
        plotLabel->setStyleSheet("background: #f6efe8; border: 1px solid #eadfd4;");
        simplifiedBtn->setStyleSheet(simplifiedBtn->isChecked()
            ? "padding: 6px 16px; border-radius: 16px; background: #f4d2c0;"
            : "padding: 6px 16px; border-radius: 16px; background: #f6efe8;");
        detailedBtn->setStyleSheet(detailedBtn->isChecked()
            ? "padding: 6px 16px; border-radius: 16px; background: #f4d2c0;"
            : "padding: 6px 16px; border-radius: 16px; background: #f6efe8;");
    }
}

int MainWindow::calculateMaxBeers(double weightKg, Gender gender, double hoursUntilTarget) const {
    // Calculate max beers (0.5l, 5% ABV) to be sober by target time
    // BAC = (alcohol_grams / (r * weight)) - beta * time = 0
    // alcohol_grams = beta * time * r * weight
    const double r = BacModel::rFactor(gender);
    const double beta = BacModel::BETA_PER_MILLE_PER_HOUR;
    const double maxAlcoholGrams = beta * hoursUntilTarget * r * weightKg;
    const double beerGrams = BacModel::alcoholGrams(500.0, 5.0); // 0.5l beer at 5%
    return static_cast<int>(maxAlcoholGrams / beerGrams);
}

double MainWindow::totalAlcoholGramsFromTable() const {
    double grams = 0.0;
    for (int i = 0; i < drinksTable->rowCount(); ++i) {
        const int count = drinksTable->item(i, 2)->text().toInt();
        const double volume = drinksTable->item(i, 3)->text().toDouble();
        const double abv = drinksTable->item(i, 4)->text().toDouble();
        grams += BacModel::alcoholGrams(volume, abv) * count;
    }
    return grams;
}

void MainWindow::onCalculate() {
    const double weightRaw = weightEdit->text().toDouble();
    if (weightRaw <= 0) {
        QMessageBox::warning(this, "Chybné údaje", "Zadej hmotnost.");
        return;
    }

    double weightKg = weightRaw;
    if (weightUnit->currentText() == "lb") {
        weightKg = BacModel::lbToKg(weightRaw);
    }

    const Gender gender = maleBtn->isChecked() ? Gender::Male : Gender::Female;

    if (endTimeEdit->dateTime() < startTimeEdit->dateTime()) {
        QMessageBox::warning(this, "Chybné údaje", "Konec pití nesmí být před začátkem.");
        return;
    }

    const double hoursElapsed = startTimeEdit->dateTime().secsTo(endTimeEdit->dateTime()) / 3600.0;

    BacResult res{};
    double totalAlcoholGrams = 0.0;

    if (modeStack->currentIndex() == 0) {
        const int drinks = simpleDrinkCount->value();
        const double standardDrinkGrams = BacModel::alcoholGrams(500.0, 5.0); // zhruba 0,5 l piva 5%
        totalAlcoholGrams = standardDrinkGrams * drinks;
        res = BacModel::computeFromGrams(gender, weightKg, totalAlcoholGrams, hoursElapsed);
    } else {
        totalAlcoholGrams = totalAlcoholGramsFromTable();
        res = BacModel::computeFromGrams(gender, weightKg, totalAlcoholGrams, hoursElapsed);
    }

    resultBac->setText(QString("Aktuální odhad: %1 ‰").arg(res.bac_per_mille, 0, 'f', 2));
    resultDrive->setText(QString("Řízení: %1").arg(res.allowed_to_drive ? "ANO" : "NE"));
    resultSober->setText(QString("Do vystřízlivění: %1 h").arg(res.hours_to_sober, 0, 'f', 1));

    // Calculate sober date/time
    QDateTime soberDateTime = endTimeEdit->dateTime().addSecs(static_cast<qint64>(res.hours_to_sober * 3600));
    resultSoberTime->setText(QString("Střízlivý v: %1").arg(soberDateTime.toString("dd.MM.yyyy HH:mm")));

    // Check target sober time if enabled
    if (targetSoberCheck->isChecked()) {
        QDateTime targetTime = targetSoberEdit->dateTime();
        if (soberDateTime <= targetTime) {
            resultTargetStatus->setText("Stihnete být střízlivý včas");
            resultTargetStatus->setStyleSheet("color: green; font-weight: 600;");
        } else {
            qint64 diffSecs = targetTime.secsTo(soberDateTime);
            double diffHours = diffSecs / 3600.0;
            resultTargetStatus->setText(QString("Nestihnete o %1 h").arg(diffHours, 0, 'f', 1));
            resultTargetStatus->setStyleSheet("color: red; font-weight: 600;");
        }

        // Calculate BAC at target time
        double hoursFromEndToTarget = endTimeEdit->dateTime().secsTo(targetTime) / 3600.0;
        double bacAtTarget = BacModel::bacAtTimeHoursFromGrams(gender, weightKg, totalAlcoholGrams, hoursElapsed + hoursFromEndToTarget);
        if (bacAtTarget <= 0.0) {
            resultTargetDrive->setText("Řízení v cílový čas: ANO");
            resultTargetDrive->setStyleSheet("color: green; font-weight: 600;");
        } else {
            resultTargetDrive->setText(QString("Řízení v cílový čas: NE (%1 ‰)").arg(bacAtTarget, 0, 'f', 2));
            resultTargetDrive->setStyleSheet("color: red; font-weight: 600;");
        }

        // Calculate max beers for target time (if no drinks yet)
        double hoursFromStartToTarget = startTimeEdit->dateTime().secsTo(targetTime) / 3600.0;
        if (hoursFromStartToTarget > 0) {
            int maxBeers = calculateMaxBeers(weightKg, gender, hoursFromStartToTarget);
            resultMaxBeers->setText(QString("Max. velkých piv (0,5l) do cíle: %1").arg(maxBeers));
        } else {
            resultMaxBeers->setText("");
        }
    } else {
        resultTargetStatus->setText("");
        resultTargetDrive->setText("");
        resultMaxBeers->setText("");
    }

    // Build time series for plot (0..max hours until sober)
    const double totalHours = hoursElapsed + res.hours_to_sober;
    const int steps = std::max(1, static_cast<int>(std::ceil(totalHours)));

    const QString dataPath = "bac_data.csv";
    QFile file(dataPath);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        out << "hour,bac\n";
        for (int h = 0; h <= steps; ++h) {
            double t = static_cast<double>(h);
            double bac = BacModel::bacAtTimeHoursFromGrams(gender, weightKg, totalAlcoholGrams, t);
            out << t << "," << bac << "\n";
        }
    }

    PythonPlotter plotter;
    const std::string pngPath = plotter.generatePlotPng(dataPath.toStdString(), "bac_plot.png");
    QPixmap pix(QString::fromStdString(pngPath));
    if (!pix.isNull()) {
        plotLabel->setPixmap(pix.scaled(plotLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    } else {
        plotLabel->setText("Graf se nepodařilo vykreslit.");
    }
}
