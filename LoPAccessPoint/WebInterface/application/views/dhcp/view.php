<?php foreach ($dhcp_leases as $dhcp_lease): ?>

    <h2><?php echo $dhcp_lease['ip_address'] ?></h2>
    <div id="main">
        <?php echo $dhcp_lease['client_id'] ?>
    </div>
    <div id="main">
        <?php echo $dhcp_lease['timestamp'] ?>
    </div>
<?php endforeach ?>