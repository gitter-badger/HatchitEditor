/**
**    Hatchit Editor
**    Copyright(c) 2015 Third-Degree
**
**    GNU General Public License
**    This file may be used under the terms of 
**    the GNU General Public License version 3 as published by the Free
**    Software Foundation and appearing in the file LICENSE.GPLv3 included
**    in the packaging of this file. Please review the following information
**    to ensure the GNU General Public License requirements
**    will be met: https://www.gnu.org/licenses/gpl.html
**
**/

#pragma once

#include <QDialog>
#include <QTabWidget>
#include <QTextEdit>
#include <QMenuBar>

#include <include/unused/ht_editor_rootlayouttree.h>

namespace Hatchit
{
    namespace Editor
    {
        class RootLayoutDialog : public QDialog
        {
            Q_OBJECT
        public:
            RootLayoutDialog(QWidget* parent = 0);

        protected slots:
            void OnFileOpen();
            void OnFileSave();

        private:
            QTabWidget*     m_tabs;
            QTextEdit*      m_textEdit;
            RootLayoutTree* m_tree;
           
        };
    }
}
