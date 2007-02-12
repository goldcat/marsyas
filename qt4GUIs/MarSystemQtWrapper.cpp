/* Filename: MarSystemQtWrapper.h
 */

#include "MarSystemQtWrapper.h"


MarSystemQtWrapper::MarSystemQtWrapper(MarSystem* msys)
{
  main_pnet_ = msys;
  running_ = false;
  pause_ = true;
  empty_ = false;
  abort_ = false;
}


MarSystemQtWrapper::~MarSystemQtWrapper()
{
 abort_ = true;
 wait();
}

MarControlPtr 
MarSystemQtWrapper::getctrl(string cname)
{
  mutex_.lock();
  MarControlPtr value;
  value = main_pnet_->getctrl(cname);
  mutex_.unlock();
  
  return value;

  
} 


void 
MarSystemQtWrapper::updctrl(MarControlPtr cname, 
			  MarControlPtr cvalue)
{
  if ( !running_ )
    {
      main_pnet_->updctrl(cname, cvalue);
      emit ctrlChanged(cname, cvalue);
    } 
  else 
    {
      mutex_.lock();
      control_names_.push_back(cname);
      control_values_.push_back(cvalue);
      emit ctrlChanged(cname, cvalue);
      mutex_.unlock();
    } 
  
} 


void 
MarSystemQtWrapper::pause()
{ 
  mutex_.lock();
  main_pnet_->updctrl("mrs_bool/active", false);
  pause_ = true;
  mutex_.unlock();
} 

void 
MarSystemQtWrapper::play()
{
  mutex_.lock();
  main_pnet_->updctrl("mrs_bool/active", true);
  pause_ = false;
  mutex_.unlock();
} 

void 
MarSystemQtWrapper::run() 
{
  while(1) 
    {
      running_ = true;
      // udpate stored controls
      mutex_.lock();
      vector<MarControlPtr>::iterator vsi;
      vector<MarControlPtr>::iterator vvi;

      if (abort_) 
	    return;
      
      for (vsi = control_names_.begin(),
	     vvi = control_values_.begin();
	   vsi != control_names_.end(); ++vsi, ++vvi)
	{
	  
	  main_pnet_->updctrl(*vsi, *vvi);
	} 
      
      control_names_.clear();
      control_values_.clear();
      mutex_.unlock();
      
      // now we are ready to tick the network
      if (pause_) 
         msleep(300);	
      else 
         main_pnet_->tick();	
      

    }
}



