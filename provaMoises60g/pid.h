#ifndef PID_H
#define PID_H

class Pid {

private:
  double kp=0;
  double ki=0;
  double kd=0;

  double setpoint = 0;
  double input;
  double output;
  double integral;
  double error;
  double prev_error;

  double maxint;
  
  unsigned long prev_time;

public:
  Pid(double kp, double ki, double kd);

  void set_input(double value);
  double get_output(void);
  double get_error(void);
  void set_setpt(double value);
};

Pid::Pid(double kp, double ki, double kd) {
  this->kp = kp;
  this->ki = ki;
  this->kd = kd;

  this->prev_time = millis();
  this->prev_error = 0;

  this->maxint = 100/this->ki;
}

void Pid::set_setpt(double value){
  this->setpoint = value;
}

double Pid::get_output() {
  return this->output;
}

double Pid::get_error() {
  return this->error;
}

void Pid::set_input(double value){

  unsigned long current_time = millis();
  unsigned long elapsed_time = current_time - this->prev_time;
  this->prev_time = current_time;
  
  this->input = value;
  this->error = this->setpoint - value;


  this->integral += this->error*(float)elapsed_time;
  this->integral = this->integral<0? 0 : this->integral;
  this->integral = this->integral>maxint? maxint : this->integral;

  
  double derivada = (this->error - this->prev_error)/(float)elapsed_time;
  this->prev_error = this->error;

  this->output = this->kp*this->error + this->ki*this->integral + this->kd*derivada;
  
}



#endif
