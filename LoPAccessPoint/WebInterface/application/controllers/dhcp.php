<?php
class Dhcp extends CI_Controller {

	public function __construct()
	{
		parent::__construct();
		$this->load->model('dhcp_model');
	}

	public function index()
	{
        //$this->output->enable_profiler(TRUE);
		$data['dhcp_leases'] = $this->dhcp_model->get_leases();
        
    	$data['title'] = "DHCP Status";
        
        $this->load->view('templates/header', $data);
    	$this->load->view('dhcp/view', $data);
    	$this->load->view('templates/footer');
	}

	public function view($slug)
	{
	    
	}
}