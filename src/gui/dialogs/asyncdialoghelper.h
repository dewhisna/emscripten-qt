#ifndef ASYNCDIALOGHELPER
#define ASYNCDIALOGHELPER

#include <QInputDialog>
#include <QColorDialog>
#include <QFontDialog>
#include <QFileDialog>
#include <QMessageBox>
#include <QIcon>
#include <QDrag>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Gui)

namespace AsyncDialogHelper
{
    namespace Private
    {
        // Rather annoyingly, QMessageBox doesn't emit a signal telling us which StandardButton was chosen
        // (only the QAbstractButton).  This class translates from QAbstractButton to StandardButton and
        // forwards the result. It is also able to translate the QAbstractButton to value that would be
        // returned from QMessageBox::exec().
        // TODO - name of the class is inaccurate, now - find a better name!
        class AbstractButtonToStandardButton : public QObject
        {
            Q_OBJECT
        public:
            AbstractButtonToStandardButton(QObject* parent) : QObject(parent)
            {
            };
        public slots:
            void forwardStandardButton(QAbstractButton* abstractButton)
            {
                QMessageBox* sendingMessageBox = qobject_cast<QMessageBox*>(sender());
                Q_ASSERT(sendingMessageBox);
                QMessageBox::StandardButton standardButton = sendingMessageBox->standardButton(abstractButton);
                emit standardButtonClicked(standardButton);
            }
            void forwardExecResult(QAbstractButton* button)
            {
                QMessageBox* sendingMessageBox = qobject_cast<QMessageBox*>(sender());
                Q_ASSERT(sendingMessageBox);
                int result = sendingMessageBox->standardButton(button);
                if (result == QMessageBox::NoButton) {
                    result = sendingMessageBox->buttons().indexOf(button); // if button == 0, correctly sets ret = -1
                }
                emit execResult(result);
            }
        signals:
            void standardButtonClicked(QMessageBox::StandardButton standardButton);
            void execResult(int execResult);
        };

        // TODO - better name for this.
        class CancelToInvalidValue : public QObject
        {
            Q_OBJECT
        public:
            CancelToInvalidValue(QObject* parent) : QObject(parent) {};
        public slots:
            void finished(int result)
            {
               bool resultWasAccept = true;
               QMessageBox *senderAsMessageBox = qobject_cast<QMessageBox*>(sender());
               if (senderAsMessageBox)
               {
                   // result is a StandardButton
                   QAbstractButton *button = senderAsMessageBox->button(static_cast<QMessageBox::StandardButton>(result));
                   if (senderAsMessageBox->buttonRole(button) == QMessageBox::RejectRole ||
                       senderAsMessageBox->buttonRole(button) == QMessageBox::InvalidRole)
                   {
                       resultWasAccept = false;
                   }
               }
               else
               {
                   resultWasAccept = (result == QDialog::Accepted);
               }

               if (!resultWasAccept)
               {
                   emit emptyString(QString());
                   emit emptyStringList(QStringList());
               }
            }
        signals:
            void emptyString(const QString& emptyString);
            void emptyStringList(const QStringList& emptyString);
        };

        inline void showMessageBox(QMessageBox::Icon icon, QObject* receiver, const char* slot, QWidget *parent, const QString &title,
                         const QString& text, QMessageBox::StandardButtons buttons = QMessageBox::Ok,
                         QMessageBox::StandardButton defaultButton = QMessageBox::NoButton)
        {
            QMessageBox *messageBox = new QMessageBox(icon, title, text, buttons, parent);
            messageBox->setModal(true);

            Private::AbstractButtonToStandardButton *abstractButtonToStandardButton = new Private::AbstractButtonToStandardButton(messageBox);

            if (receiver)
            {
                QObject::connect(messageBox, SIGNAL(buttonClicked(QAbstractButton*)), abstractButtonToStandardButton, SLOT(forwardStandardButton(QAbstractButton*)));
                QObject::connect(abstractButtonToStandardButton, SIGNAL(standardButtonClicked(QMessageBox::StandardButton)),
                                receiver, slot);
            }
            QObject::connect(messageBox, SIGNAL(finished(int)), messageBox, SLOT(deleteLater()));
            messageBox->show();
        }
    }
    inline void getInt(QObject* receiver, const char* slot, QWidget * parent, const QString & title, const QString & label, int value = 0, int min = -2147483647, int max = 2147483647, int step = 1, Qt::WindowFlags flags = 0)
    {
        QInputDialog *inputDialog = new QInputDialog(parent, flags);
        inputDialog->setWindowTitle(title);
        inputDialog->setLabelText(label);
        inputDialog->setIntValue(value);
        inputDialog->setIntMinimum(min);
        inputDialog->setIntMaximum(max);
        inputDialog->setIntStep(step);
        inputDialog->setModal(true);
        QObject::connect(inputDialog, SIGNAL(intValueSelected(int)), receiver, slot);
        QObject::connect(inputDialog, SIGNAL(finished(int)), inputDialog, SLOT(deleteLater()));
        inputDialog->show();
    }
    inline void getDouble (QObject* receiver, const char* slot, QWidget * parent, const QString & title, const QString & label, double value = 0, double min = -2147483647, double max = 2147483647, int decimals = 1, Qt::WindowFlags flags = 0 )
    {
        QInputDialog *inputDialog = new QInputDialog(parent, flags);
        inputDialog->setWindowTitle(title);
        inputDialog->setLabelText(label);
        inputDialog->setDoubleValue(value);
        inputDialog->setDoubleMinimum(min);
        inputDialog->setDoubleMaximum(max);
        inputDialog->setDoubleDecimals(decimals);
        inputDialog->setModal(true);
        QObject::connect(inputDialog, SIGNAL(doubleValueSelected(double)), receiver, slot);
        QObject::connect(inputDialog, SIGNAL(finished(int)), inputDialog, SLOT(deleteLater()));
        inputDialog->show();
    }
    inline void getItem (QObject* receiver, const char* slot, QWidget * parent, const QString & title, const QString & label, const QStringList & items, int current = 0, bool editable = true, Qt::WindowFlags flags = 0, Qt::InputMethodHints inputMethodHints = Qt::ImhNone )
    {
        QString text(items.value(current));

        QInputDialog *inputDialog = new QInputDialog(parent, flags);
        inputDialog->setWindowTitle(title);
        inputDialog->setLabelText(label);
        inputDialog->setComboBoxItems(items);
        inputDialog->setTextValue(text);
        inputDialog->setComboBoxEditable(editable);
        inputDialog->setInputMethodHints(inputMethodHints);
        inputDialog->setModal(true);
        QObject::connect(inputDialog, SIGNAL(textValueSelected(const QString&)), receiver, slot);
        QObject::connect(inputDialog, SIGNAL(finished(int)), inputDialog, SLOT(deleteLater()));
        inputDialog->show();
    }
    inline void getText (QObject* receiver, const char* slot, QWidget * parent, const QString & title, const QString & label, QLineEdit::EchoMode mode = QLineEdit::Normal, const QString & text = QString(), bool * ok = 0, Qt::WindowFlags flags = 0, Qt::InputMethodHints inputMethodHints = Qt::ImhNone )
    {
        QInputDialog *inputDialog = new QInputDialog(parent, flags);
        inputDialog->setWindowTitle(title);
        inputDialog->setLabelText(label);
        inputDialog->setTextValue(text);
        inputDialog->setTextEchoMode(mode);
        inputDialog->setInputMethodHints(inputMethodHints);
        inputDialog->setModal(true);
        QObject::connect(inputDialog, SIGNAL(textValueSelected(const QString&)), receiver, slot);
        QObject::connect(inputDialog, SIGNAL(finished(int)), inputDialog, SLOT(deleteLater()));
        inputDialog->show();
    }
    inline void getColor(QObject* receiver, const char* slot, const QColor &initial, QWidget *parent = 0, const QString &title = QString(), QColorDialog::ColorDialogOptions options = 0)
    {
        QColorDialog *colorDialog = new QColorDialog(parent);
        if (!title.isEmpty())
            colorDialog->setWindowTitle(title);
        colorDialog->setOptions(options);
        colorDialog->setCurrentColor(initial);
        colorDialog->setModal(true);
        QObject::connect(colorDialog, SIGNAL(colorSelected(const QColor&)), receiver, slot);
        QObject::connect(colorDialog, SIGNAL(finished(int)), colorDialog, SLOT(deleteLater()));
        colorDialog->show();
    }
    inline void getFont(QObject* receiver, const char* slot, const QFont &initial, QWidget *parent,
                                  const QString &title = QString(), QFontDialog::FontDialogOptions options = 0)
    {
        QFontDialog *fontDialog = new QFontDialog(parent);
        fontDialog->setOptions(options);
        fontDialog->setCurrentFont(initial);
        fontDialog->setModal(true);
        if (!title.isEmpty())
            fontDialog->setWindowTitle(title);
        QObject::connect(fontDialog, SIGNAL(fontSelected(const QFont&)), receiver, slot);
        QObject::connect(fontDialog, SIGNAL(finished(int)), fontDialog, SLOT(deleteLater()));
        fontDialog->show();
    }
    inline void getExistingDirectory(QObject* receiver, const char* slot, QWidget *parent,
                                          const QString &caption,
                                          const QString &dir,
                                          QFileDialog::Options options)
    {
        QFileDialog *fileDialog = new QFileDialog(parent, caption, QDir::currentPath());
        fileDialog->setOptions(options);
        fileDialog->setFileMode(options & QFileDialog::ShowDirsOnly ? QFileDialog::DirectoryOnly : QFileDialog::Directory);
        fileDialog->setModal(true);
        QObject::connect(fileDialog, SIGNAL(fileSelected(const QString&)), receiver, slot);
        QObject::connect(fileDialog, SIGNAL(finished(int)), fileDialog, SLOT(deleteLater()));
        fileDialog->show();
    }
    inline void getOpenFileName(QObject* receiver, const char* slot, QWidget *parent = 0,
                                const QString &caption = QString(),
                                const QString &dir = QString(),
                                const QString &filter = QString(),
                                const QString& selectedFilter = QString(),
                                QFileDialog::Options options = 0)
    {
        QFileDialog *fileDialog = new QFileDialog(parent, caption, dir);
        fileDialog->setOptions(options);
        fileDialog->setFileMode(QFileDialog::ExistingFile);
        fileDialog->setFilter(filter);
        fileDialog->setModal(true);
        if (!selectedFilter.isEmpty())
            fileDialog->selectNameFilter(selectedFilter);
        QObject::connect(fileDialog, SIGNAL(fileSelected(const QString&)), receiver, slot);
        QObject::connect(fileDialog, SIGNAL(finished(int)), fileDialog, SLOT(deleteLater()));
        fileDialog->show();
    }
    inline void getOpenFileNames(QObject* receiver, const char* slot, QWidget *parent = 0,
                                const QString &caption = QString(),
                                const QString &dir = QString(),
                                const QString &filter = QString(),
                                const QString& selectedFilter = QString(),
                                QFileDialog::Options options = 0)
    {
        QFileDialog *fileDialog = new QFileDialog(parent, caption, dir);
        fileDialog->setOptions(options);
        fileDialog->setFileMode(QFileDialog::ExistingFiles);
        fileDialog->setFilter(filter);
        fileDialog->setModal(true);
        if (!selectedFilter.isEmpty())
            fileDialog->selectNameFilter(selectedFilter);
        QObject::connect(fileDialog, SIGNAL(filesSelected(const QStringList&)), receiver, slot);
        QObject::connect(fileDialog, SIGNAL(finished(int)), fileDialog, SLOT(deleteLater()));
        fileDialog->show();
    }
    inline void getSaveFileName(QObject* receiver, const char* slot, QWidget *parent = 0,
                                const QString &caption = QString(),
                                const QString &dir = QString(),
                                const QString &filter = QString(),
                                const QString& selectedFilter = QString(),
                                QFileDialog::Options options = 0)
    {
        QFileDialog *fileDialog = new QFileDialog(parent, caption, dir);
        fileDialog->setAcceptMode(QFileDialog::AcceptSave);
        fileDialog->setOptions(options);
        fileDialog->setFileMode(QFileDialog::AnyFile);
        fileDialog->setFilter(filter);
        fileDialog->setModal(true);
        if (!selectedFilter.isEmpty())
            fileDialog->selectNameFilter(selectedFilter);

        Private::CancelToInvalidValue *cancelToInvalidValueMapper = new Private::CancelToInvalidValue(fileDialog);
        QObject::connect(fileDialog, SIGNAL(fileSelected(const QString&)), receiver, slot);
        QObject::connect(fileDialog, SIGNAL(finished(int)), cancelToInvalidValueMapper, SLOT(finished(int)));
        QObject::connect(cancelToInvalidValueMapper, SIGNAL(emptyString(const QString&)), receiver, slot);
        QObject::connect(fileDialog, SIGNAL(finished(int)), fileDialog, SLOT(deleteLater()));
        fileDialog->show();
    }
    inline void critical(QObject* receiver, const char* slot, QWidget *parent, const QString &title,
                         const QString& text, QMessageBox::StandardButtons buttons = QMessageBox::Ok,
                         QMessageBox::StandardButton defaultButton = QMessageBox::NoButton)
    {
        Private::showMessageBox(QMessageBox::Critical, receiver, slot, parent, title, text, buttons, defaultButton);
    }
    inline void information(QObject* receiver, const char* slot, QWidget *parent, const QString &title,
                         const QString& text, QMessageBox::StandardButtons buttons = QMessageBox::Ok,
                         QMessageBox::StandardButton defaultButton = QMessageBox::NoButton)
    {
        Private::showMessageBox(QMessageBox::Information, receiver, slot, parent, title, text, buttons, defaultButton);
    }
    inline void question(QObject* receiver, const char* slot, QWidget *parent, const QString &title,
                         const QString& text, QMessageBox::StandardButtons buttons = QMessageBox::Ok,
                         QMessageBox::StandardButton defaultButton = QMessageBox::NoButton)
    {
        Private::showMessageBox(QMessageBox::Question, receiver, slot, parent, title, text, buttons, defaultButton);
    }
    inline void warning(QObject* receiver, const char* slot, QWidget *parent, const QString &title,
                         const QString& text, QMessageBox::StandardButtons buttons = QMessageBox::Ok,
                         QMessageBox::StandardButton defaultButton = QMessageBox::NoButton)
    {
        Private::showMessageBox(QMessageBox::Warning, receiver, slot, parent, title, text, buttons, defaultButton);
    }
    inline void exec(QObject* receiver, const char* slot, QMessageBox* messageBox)
    {
            Private::AbstractButtonToStandardButton *abstractButtonToStandardButton = new Private::AbstractButtonToStandardButton(messageBox);

            messageBox->setModal(true);
            QObject::connect(messageBox, SIGNAL(buttonClicked(QAbstractButton*)), abstractButtonToStandardButton, SLOT(forwardExecResult(QAbstractButton*)));
            QObject::connect(abstractButtonToStandardButton, SIGNAL(execResult(int)), receiver, slot);
            QObject::connect(messageBox, SIGNAL(finished(int)), messageBox, SLOT(deleteLater()));
            messageBox->show();
    }
    inline void about (QWidget * parent, const QString & title, const QString & text )
    {
        QMessageBox *messageBox = new QMessageBox(title, text, QMessageBox::Information, 0, 0, 0, parent);
        QIcon icon = messageBox->windowIcon();
        QSize size = icon.actualSize(QSize(64, 64));
        messageBox->setIconPixmap(icon.pixmap(size));
        messageBox->setModal(true);
        QObject::connect(messageBox, SIGNAL(finished(int)), messageBox, SLOT(deleteLater()));
        messageBox->show();
    }
    inline void aboutQt(QWidget *parent, const QString &title = QString())
    {
        QString translatedTextAboutQtCaption;
        translatedTextAboutQtCaption = QMessageBox::tr(
            "<h3>About Qt</h3>"
            "<p>This program uses Qt version %1.</p>"
            ).arg(QLatin1String(QT_VERSION_STR));
        QString translatedTextAboutQtText;
        translatedTextAboutQtText = QMessageBox::tr(
            "<p>Qt is a C++ toolkit for cross-platform application "
            "development.</p>"
            "<p>Qt provides single-source portability across MS&nbsp;Windows, "
            "Mac&nbsp;OS&nbsp;X, Linux, and all major commercial Unix variants. "
            "Qt is also available for embedded devices as Qt for Embedded Linux "
            "and Qt for Windows CE.</p>"
            "<p>Qt is available under three different licensing options designed "
            "to accommodate the needs of our various users.</p>"
            "<p>Qt licensed under our commercial license agreement is appropriate "
            "for development of proprietary/commercial software where you do not "
            "want to share any source code with third parties or otherwise cannot "
            "comply with the terms of the GNU LGPL version 2.1 or GNU GPL version "
            "3.0.</p>"
            "<p>Qt licensed under the GNU LGPL version 2.1 is appropriate for the "
            "development of Qt applications (proprietary or open source) provided "
            "you can comply with the terms and conditions of the GNU LGPL version "
            "2.1.</p>"
            "<p>Qt licensed under the GNU General Public License version 3.0 is "
            "appropriate for the development of Qt applications where you wish to "
            "use such applications in combination with software subject to the "
            "terms of the GNU GPL version 3.0 or where you are otherwise willing "
            "to comply with the terms of the GNU GPL version 3.0.</p>"
            "<p>Please see <a href=\"http://qt.digia.com/product/licensing\">qt.digia.com/product/licensing</a> "
            "for an overview of Qt licensing.</p>"
            "<p>Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).</p>"
            "<p>Qt is a Digia product. See <a href=\"http://qt.digia.com/\">qt.digia.com</a> "
            "for more information.</p>"
            );
        QMessageBox *messageBox = new QMessageBox(parent);
        messageBox->setWindowTitle(title.isEmpty() ? QMessageBox::tr("About Qt") : title);
        messageBox->setText(translatedTextAboutQtCaption);
        messageBox->setInformativeText(translatedTextAboutQtText);

        QPixmap pm(QLatin1String(":/trolltech/qmessagebox/images/qtlogo-64.png"));
        if (!pm.isNull())
            messageBox->setIconPixmap(pm);
        messageBox->setModal(true);
        QObject::connect(messageBox, SIGNAL(finished(int)), messageBox, SLOT(deleteLater()));
        messageBox->show();
    }
    inline void startDrag(QObject* receiver, const char* slot, QDrag* drag, Qt::DropActions supportedActions = Qt::MoveAction)
    {
#ifdef QT_NO_LOCALEVENTLOOP
        if (receiver)
        {
            QObject::connect(drag, SIGNAL(asyncDragFinished(Qt::DropAction)), receiver, slot);
        }
        drag->startAsyncDrag(supportedActions);
#else
        // TODO - non-async version
#endif
    }
}


QT_END_NAMESPACE

QT_END_HEADER

#endif