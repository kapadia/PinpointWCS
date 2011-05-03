/*
 *  PinpointWCS is developed by the Chandra X-ray Center
 *  Education and Public Outreach Group
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "HelpPanel.h"
#include <QDebug>

HelpPanel::HelpPanel(QWidget *parent)
: QFrame(parent)
{
	// Setup user interface
    ui.setupUi(this);
	
	// Set frame style
	setFrameStyle(QFrame::Panel | QFrame::StyledPanel);
	
	// Set colors
	setAutoFillBackground(true);
	setBackgroundRole(QPalette::Window);
	QPalette palette;
	QColor bgcolor = QColor(0, 0, 0, 200);
	QColor textcolor = QColor(230, 230, 230);
	palette.setColor(QPalette::Background, bgcolor);
	palette.setColor(QPalette::WindowText, textcolor);
	setPalette(palette);
	
	// Adjust the style of the text edit box
	ui.textEdit->setTextBackgroundColor(bgcolor);
	ui.textEdit->setTextColor(textcolor);

}

HelpPanel::~HelpPanel() {}


void HelpPanel::parentResized(QSize sz)
{
	resize(sz.width(), sz.height()-20);
}