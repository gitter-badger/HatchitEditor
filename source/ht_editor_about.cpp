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

#include <ht_editor_about.h>
#include <ht_editor_version.h>
#include <ht_os.h>

#include <QBoxLayout>
#include <QFile>


namespace Hatchit {

    namespace Editor {

        AboutDialog::AboutDialog(QWidget* widget)
            : QDialog(widget)
        {
            resize(1080, 720);
            setWindowTitle(tr("About Hatchit Editor"));

            m_webView = new QWebView;
            m_firstLoad = true;
            connect(m_webView, SIGNAL(loadFinished(bool)), this, SLOT(OnLoadLicense(bool)));
            m_webView->load(QUrl("http://www.gnu.org/licenses/gpl.html"));
            
            QString aboutText = QString("Hatchit Editor\nVersion %1.%2.%3\n").arg(HatchitEditor_VERSION_MAJOR)
                                                                           .arg(HatchitEditor_VERSION_MINOR)
                                                                           .arg(HatchitEditor_VERSION_BUILD);

            QString creditsText = QString("Copyright(c) 2015 Third - Degree\nCopyright(c) 2015 Matt Guerrette");

            QTextBrowser* aboutBrowser = new QTextBrowser;
            aboutBrowser->setText(aboutText + "\n" + creditsText);
            m_aboutPane = new CollapsePane("About", aboutBrowser, false);

   

            m_mainLayout = new QVBoxLayout;
            m_mainLayout->addWidget(m_aboutPane);
        }


        void AboutDialog::OnLoadLicense(bool ok)
        {
            
            if (m_firstLoad)
            {
                if (!ok)
                {
                    QTextBrowser* browser = new QTextBrowser;
                    QFile license(QString::fromStdString(Hatchit::Core::os_exec_dir()) + "LICENSE.GPLv3");
                    license.open(QIODevice::ReadOnly);
                    browser->setText(license.readAll());

                    m_collapsePane = new CollapsePane(tr("License"), browser, false);
                }
                else
                {
                    m_collapsePane = new CollapsePane(tr("License"), m_webView, false);
                }

                
                m_mainLayout->addWidget(m_collapsePane);
                m_mainLayout->addStretch();
                setLayout(m_mainLayout);
                
            }

            m_firstLoad = false;
        }

    }

}