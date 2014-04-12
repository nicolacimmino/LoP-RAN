<?php
class Dhcp_model extends CI_Model {

	public function __construct()
	{
		$this->load->database();
	}

    public function get_leases()
    {
        $query = $this->db->get('dhcp_leases');
		return $query->result_array();
    }
}

?>