/*
** Copyright (C) 1998-2006 George Tzanetakis <gtzan@cs.uvic.ca>
**  
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
** 
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
** 
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software 
** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/

#include "OnsetTimes.h"

using namespace std;
using namespace Marsyas;

OnsetTimes::OnsetTimes(string name):MarSystem("OnsetTimes", name)
{
  addControls();
  t_ = 0;
  count_ = 0;
}

OnsetTimes::OnsetTimes(const OnsetTimes& a) : MarSystem(a)
{
  // For any MarControlPtr in a MarSystem 
  // it is necessary to perform this getctrl 
  // in the copy constructor in order for cloning to work 
	ctrl_n1stOnsets_ = getctrl("mrs_natural/n1stOnsets");
	ctrl_lookAheadSamples_ = getctrl("mrs_natural/lookAheadSamples");
	ctrl_nPeriods_ = getctrl("mrs_natural/nPeriods");
	
	t_ = a.t_;
	count_ = a.count_;
}

OnsetTimes::~OnsetTimes()
{
}

MarSystem* 
OnsetTimes::clone() const 
{
  return new OnsetTimes(*this);
}

void 
OnsetTimes::addControls()
{
  //Add specific controls needed by this MarSystem.
	addctrl("mrs_natural/n1stOnsets", 1, ctrl_n1stOnsets_);
	setctrlState("mrs_natural/n1stOnsets", true);
	addctrl("mrs_natural/lookAheadSamples", 1, ctrl_lookAheadSamples_);
	addctrl("mrs_natural/nPeriods", 1, ctrl_nPeriods_);
	setctrlState("mrs_natural/nPeriods", true);
}

void
OnsetTimes::myUpdate(MarControlPtr sender)
{
	MRSDIAG("OnsetTimes.cpp - OnsetTimes:myUpdate");
	n_ = ctrl_n1stOnsets_->to<mrs_natural>();
	nPeriods_ = ctrl_nPeriods_->to<mrs_natural>();	
	
	//if the nr of requested bpm hypotheses is bigger
	//than the requested phases we need to fill the rest of the vector with
	//0s -> to avoid crash in tempohypotheses Fanout
	mrs_natural size = 2 * max(n_, nPeriods_);
	setctrl("mrs_natural/onSamples",  size);
	
	ctrl_onObservations_->setValue(1, NOUPDATE);
	ctrl_osrate_->setValue(ctrl_israte_, NOUPDATE);


}


void 
OnsetTimes::myProcess(realvec& in, realvec& out)
{
	lookAhead_ = ctrl_lookAheadSamples_->to<mrs_natural>();
	
	t_++;
	
	mrs_natural inc = 1; //nr. of first ignored onsets
	if(in(0,0) == 1.0){
		//if task isn't still done && (first peak || peak distance at least 5 frames from last peak)
		if(count_ == inc || (count_ > inc && count_ < n_ + inc && t_ > out(0, 2*(count_-inc)-1) + 5))
		{
			//cout << "Out Last Arg: " << out(0, 2*(count_-inc)+1) << " Current Arg: " << t_ - lookAhead_ << endl;
			out(0, 2*(count_-inc)) = 1.0;
			out(0, 2*(count_-inc)+1) = t_ - lookAhead_-1;
		}
		count_++;
	}

	//MATLAB_PUT(out, "OnsetTimes");
}
