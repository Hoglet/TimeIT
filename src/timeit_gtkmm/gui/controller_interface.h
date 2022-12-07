

#ifndef CONTROLLER_INTERFACE_H
#define CONTROLLER_INTERFACE_H


class controller_interface
{
public:
	virtual void on_settings()   = 0;
	virtual void on_quit()       = 0;

	virtual void on_edit_task()  = 0;
	virtual void on_start()      = 0;
	virtual void on_stop()       = 0;
	virtual void on_stop_all()   = 0;
    virtual void on_add_time()   = 0;
	virtual void on_add_task()   = 0;

	virtual void on_about()      = 0;
	virtual void on_report_bug() = 0;
	virtual void on_help()       = 0;
};


#endif //CONTROLLER_INTERFACE_H
