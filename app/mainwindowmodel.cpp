#include "mainwindowmodel.h"

MainWindowModel::MainWindowModel(QObject *parent) :
    QObject(parent)
{

}

void MainWindowModel::loadFont(const QFont &font)
{
    fontFaceViewModel_ = std::make_unique<FontFaceViewModel>(font);
    setState(WithFace);
}

void MainWindowModel::setState(State s)
{
    if (state_ != s) {
        state_ = s;

        std::vector<MainWindowAction> enabledActions;
        for (int f = MainWindowAction::ActionFirst; f != MainWindowAction::ActionCount; ++f) {
            if (s & 1<<f) {
                enabledActions.push_back(MainWindowAction(f));
            }
        }
        setEnabledActions(enabledActions);
        emit actionsChanged(actionsState_);
    }
}

void MainWindowModel::setEnabledActions(std::vector<MainWindowAction> actions)
{
    actionsState_.reset();
    for (const auto &a : actions) {
        actionsState_.set(a);
    }
}
