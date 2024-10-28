#ifndef LOGWIDGET_H
#define LOGWIDGET_H

#include <QWidget>

class QTextEdit;

class LogWidget : public QWidget
{
    Q_OBJECT
public:
    explicit LogWidget(QWidget *parent = nullptr);

    // LOG TYPE
    static constexpr int INFO = 0;
    static constexpr int SUCCESS = 1;
    static constexpr int WARNING = 2;
    static constexpr int DANGER = 3;

    /**
     * @brief addLog ajouter un text au log
     * @param text: texte à ajouter
     * @param type: le type de log. les types sont:
     *  LogWidget::INFO
     *  LogWidget::SUCCESS
     *  LogWidget::WARNING
     *  LogWidget::DANGER
     * @return true si le text a été ajouté avec succés, false sinon
     */
    bool addLog(const QString& text, int type);

signals:

private:
    void createInterface();
    QTextEdit *d_logtext;
};

#endif // LOGWIDGET_H
