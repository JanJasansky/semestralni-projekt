#include "MainWindow.h"
#include "BacModel.h"
#include "PythonPlotter.h"

#include <QGridLayout>
#include <QLabel>
#include <QPixmap>
#include <QHeaderView>
#include <QFrame>
#include <QAbstractItemView>
#include <QDoubleValidator>
#include <QMessageBox>
#include <QFile>
#include <QTextStream>

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

    header->addWidget(simplifiedBtn);
    header->addWidget(detailedBtn);
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

    auto* weightLabel = new QLabel("Hmotnost");
    auto* weightRow = new QHBoxLayout;
    weightEdit = new QLineEdit;
    weightEdit->setValidator(new QDoubleValidator(0, 500, 2, this));
    weightUnit = new QComboBox;
    weightUnit->addItems({"kg", "lb"});
    weightRow->addWidget(weightEdit);
    weightRow->addWidget(weightUnit);
    basicsLayout->addWidget(weightLabel);
    basicsLayout->addLayout(weightRow);

    auto* genderLabel = new QLabel("Pohlaví");
    auto* genderRow = new QHBoxLayout;
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

    auto* startLabel = new QLabel("Začátek pití");
    startTimeEdit = new QDateTimeEdit(QDateTime::currentDateTime());
    startTimeEdit->setDisplayFormat("dd.MM.yyyy HH:mm");
    startTimeEdit->setCalendarPopup(true);

    auto* endLabel = new QLabel("Konec pití");
    endTimeEdit = new QDateTimeEdit(QDateTime::currentDateTime());
    endTimeEdit->setDisplayFormat("dd.MM.yyyy HH:mm");
    endTimeEdit->setCalendarPopup(true);

    basicsLayout->addWidget(startLabel);
    basicsLayout->addWidget(startTimeEdit);
    basicsLayout->addWidget(endLabel);
    basicsLayout->addWidget(endTimeEdit);

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

    addDrinkBtn = new QPushButton("Přidat nápoj");
    formRow->addWidget(addDrinkBtn, 1, 5);

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
    auto* calcBtn = new QPushButton("Spočítat");
    connect(calcBtn, &QPushButton::clicked, this, &MainWindow::onCalculate);

    resultsLayout->addWidget(resultBac);
    resultsLayout->addWidget(resultDrive);
    resultsLayout->addWidget(resultSober);
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
    connect(drinkTypeCombo, &QComboBox::currentTextChanged, this, &MainWindow::onDrinkTypeChanged);
    connect(drinkSizeCombo, &QComboBox::currentTextChanged, this, &MainWindow::onDrinkSizeChanged);

    updateDrinkDefaults();
}

void MainWindow::onModeSimplified() {
    simplifiedBtn->setChecked(true);
    detailedBtn->setChecked(false);
    simplifiedBtn->setStyleSheet("padding: 6px 16px; border-radius: 16px; background: #f4d2c0;");
    detailedBtn->setStyleSheet("padding: 6px 16px; border-radius: 16px; background: #f6efe8;");
    modeStack->setCurrentIndex(0);
}

void MainWindow::onModeDetailed() {
    simplifiedBtn->setChecked(false);
    detailedBtn->setChecked(true);
    simplifiedBtn->setStyleSheet("padding: 6px 16px; border-radius: 16px; background: #f6efe8;");
    detailedBtn->setStyleSheet("padding: 6px 16px; border-radius: 16px; background: #f4d2c0;");
    modeStack->setCurrentIndex(1);
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
