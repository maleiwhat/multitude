/* COPYRIGHT
 *
 * This file is part of Applications/FireView.
 *
 * Copyright: MultiTouch Oy, Helsinki University of Technology and others.
 *
 * See file "Applications/FireView.hpp" for authors and more details.
 *
 * This file is licensed under GNU Lesser General Public
 * License (LGPL), version 2.1. The LGPL conditions can be found in 
 * file "LGPL.txt" that is distributed with this source package or obtained 
 * from the GNU organization (www.gnu.org).
 * 
 */

#include "ParamView.hpp"

#include "CamView.hpp"

#include <Radiant/Video1394.hpp>

#include <QComboBox>
#include <QGridLayout>
#include <QLabel>
#include <QSlider>

namespace FireView {
  
  ParamView::ParamView(CamView * camview)
    : m_camview(camview)
  {}
    
  ParamView::~ParamView()
  {}

  void ParamView::init()
  {
    std::vector<dc1394feature_info_t> & features = m_camview->features();

    QGridLayout * layout = new QGridLayout(this);

    int row = 0;

    m_rows.resize(features.size());

    for(unsigned i = 0; i < features.size(); i++) {

      const dc1394feature_info_t & f = features[i];

      if(!f.available || !Radiant::Video1394::hasManualMode(f) || (f.min == f.max))
	continue;
      
      QSlider * slider = new QSlider(Qt::Horizontal, this);
      QLabel  * label1 = new QLabel(Radiant::Video1394::featureName(f.id), this);
      QLabel  * label2 = new QLabel(this);
      Mapper  * mapper = new Mapper(this, i);

      if(Radiant::Video1394::hasAutoMode(f)) {
	Mapper  * mapper2 = new Mapper(this, i);
	QComboBox * cb = new QComboBox(this);

	cb->addItem("Auto");
	cb->addItem("Man");
	cb->setCurrentIndex(f.current_mode == DC1394_FEATURE_MODE_AUTO ? 0 : 1);

	connect(cb, SIGNAL(activated(int)), mapper2, SLOT(setInt(int)));
	connect(mapper2, SIGNAL(emitInt(int,int)),
		this, SLOT(setAuto(int,int)));

	layout->addWidget(cb, row, 1);
	cb->show();
      }

      layout->addWidget(label1, row, 0);
      layout->addWidget(slider, row, 2);
      layout->addWidget(label2, row, 3);

      slider->setRange(f.min, f.max);
      slider->setValue(f.value);

      label2->setNum((int) f.value);

      connect(slider, SIGNAL(valueChanged(int)), mapper, SLOT(setInt(int)));
      connect(mapper, SIGNAL(emitInt(int,int)),
	      this, SLOT(sliderMoved(int,int)));

      label1->show();
      slider->show();
      label2->show();

      m_rows[i].slider = slider;
      m_rows[i].value  = label2;

      row++;
    }
  }
    
  void ParamView::sliderMoved(int index, int val)
  {
    if(m_rows[index].value)
      m_rows[index].value->setNum(val);

    m_camview->features()[index].value = val;
    m_camview->updateParam(index);
  }

  void ParamView::setAuto(int index, int value)
  {
    bool manual = (value != 0);

    m_rows[index].slider->setEnabled(manual);
    m_rows[index].value->setEnabled(manual);

    if(manual)
      m_camview->updateParam(index);
    else
      m_camview->autoParam(index);
  }
}
