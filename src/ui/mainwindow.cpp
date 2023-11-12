#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QClipboard>
#include <QElapsedTimer>
#include <QErrorMessage>
#include <QFileDialog>
#include <QMessageBox>
#include <QRegularExpression>
#include <QSortFilterProxyModel>

#include "../ModelIndexIterator.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    mSearchModel = new QSortFilterProxyModel(this);

    ui->mIncludes->setModel(mSearchModel);
    ui->mIncludes->header()->setSectionResizeMode(
        QHeaderView::ResizeToContents);
    mSearchModel->setRecursiveFilteringEnabled(true);
    connect(ui->mSearch, &QLineEdit::textChanged, this,
            [this](const QString &newText)
            {
                if (this->ui->searchRegex->isChecked())
                {
                    QRegularExpression regex(newText);
                    if (regex.isValid())
                    {
                        this->mSearchModel->setFilterRegularExpression(newText);
                        this->ui->mSearch->setStyleSheet(
                            QStringLiteral("QLineEdit {}"));
                    }
                    else
                    {
                        this->ui->mSearch->setStyleSheet(QStringLiteral(
                            "QLineEdit {background-color: red;}"));
                    }
                }
                else
                {
                    this->mSearchModel->setFilterFixedString(newText);
                }
            });

    auto loadAction = ui->menuFile->addAction(QStringLiteral("Open..."));
    loadAction->setShortcut(QKeySequence::Open);
    connect(loadAction, &QAction::triggered, this,
            [this]()
            {
                auto fileName = QFileDialog::getOpenFileName(
                    this, QStringLiteral("Open Include text file"));
                if (!fileName.isEmpty())
                    loadFromFile(fileName);
            });

    auto loadFromClipBoard = ui->menubar->addAction(QStringLiteral(""));
    loadFromClipBoard->setShortcut(QKeySequence::Paste);
    connect(loadFromClipBoard, &QAction::triggered, this,
            [this]()
            {
                try
                {
                    loadFromMemory(
                        QApplication::clipboard()->text().toStdString());
                }
                catch (...)
                {
                    // just ignore
                }
            });

    auto focusSearch = ui->menubar->addAction(QStringLiteral(""));
    focusSearch->setShortcut(QKeySequence::Find);
    connect(focusSearch, &QAction::triggered, this,
            [this]() { this->ui->mSearch->setFocus(Qt::ShortcutFocusReason); });

    ui->menuFile->addSeparator();

    auto closeAction = ui->menuFile->addAction(QStringLiteral("Close"));
    closeAction->setShortcut(QKeySequence::Close);
    connect(closeAction, &QAction::triggered, this,
            [this]() { QApplication::closeAllWindows(); });

    connect(
        ui->mSearch, &QLineEdit::returnPressed, this,
        [this]()
        {
            // get start position, either begin or selected item +1
            auto selectedIndices =
                ui->mIncludes->selectionModel()->selectedIndexes();
            const bool searchFromStart = selectedIndices.empty();
            QModelIndex startItem = (searchFromStart)
                                        ? mSearchModel->index(0, 0)
                                        : selectedIndices[0];

            auto startIter = ModelIndexIterator(*mSearchModel, startItem);
            if (!searchFromStart)
                ++startIter;

            // search for the next occurance
            std::function containsText =
                [this, regex = ui->mSearch->text()](const QModelIndex &index)
            { return mSearchModel->data(index).toString().contains(regex); };

            if (ui->searchRegex->isChecked())
            {
                containsText =
                    [this, regex = QRegularExpression(ui->mSearch->text())](
                        const QModelIndex &index)
                {
                    return mSearchModel->data(index).toString().contains(regex);
                };
            }
            auto iter =
                std::find_if(startIter, end(*mSearchModel), containsText);

            if (!searchFromStart && iter == end(*mSearchModel))
            {
                iter = std::find_if(
                    begin(*mSearchModel),
                    ModelIndexIterator(*mSearchModel, startItem), containsText);
            }

            // select and expand if found
            if (iter != end(*mSearchModel))
            {
                ui->mIncludes->selectionModel()->select(
                    *iter, QItemSelectionModel::ClearAndSelect);
                auto expandItem = *iter;
                while (expandItem.isValid())
                {
                    ui->mIncludes->expand(expandItem);
                    expandItem = expandItem.parent();
                }
                ui->mIncludes->scrollTo(*iter);
            }
        });

    connect(
        ui->actionAbout, &QAction::triggered, this,
        [this]()
        {
            QMessageBox::about(
                this, QString(QStringLiteral("IncludeFilter v%1")).arg(VERSION),
                QString(
                    QStringLiteral(
                        "IncludeFilter Version %1\n"
                        "© 2023 Matthias Möller\n"
                        "WebSite: https://github.com/TinyTinni/IncludeFilter/"))
                    .arg(VERSION));
        });
}

Parsing MainWindow::parsingMethod() const noexcept
{
    if (ui->selectClang->isChecked())
        return Parsing::CLANG;
    else
        return Parsing::MSVC;
}

void MainWindow::loadFromMemory(std::string_view mem)
{
    QElapsedTimer timer;
    timer.start();

    if (auto model = createModel(mem, parsingMethod()))
    {
        if (auto oldModel = mSearchModel->sourceModel())
        {
            oldModel->setParent(nullptr);
            delete oldModel;
        }
        model->setParent(this);
        mSearchModel->setSourceModel(model);
    }

    this->ui->statusbar->showMessage(
        QString(QStringLiteral("Loading time: %1ms")).arg(timer.elapsed()));
}

void MainWindow::loadFromFile(QString filename) noexcept
{
    try
    {
        QFile file(filename);
        if (!file.open(QIODeviceBase::ReadOnly))
        {
            throw std::runtime_error(file.errorString().toStdString());
        }

        auto file_content = [&file]()
        {
            auto file_ptr = (char *)file.map(0, file.size());
            if (file_ptr == nullptr)
            {
                throw std::runtime_error(file.errorString().toStdString());
            }
            return std::string_view{file_ptr, static_cast<size_t>(file.size())};
        };

        loadFromMemory(file_content());
    }
    catch (const std::exception &e)
    {
        QErrorMessage err;
        err.showMessage(QString::fromUtf8(e.what()));
        err.exec();
    }
}

MainWindow::~MainWindow() noexcept { delete ui; }
