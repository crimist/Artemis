<?php
	session_start();
	require_once 'include/main.php';

	/* Test if they are installed or not */
	$tableExists = $conn->query("SHOW TABLES LIKE 'users'")->rowCount();

	if ($tableExists > 0) {
		header('Location: login/index.php');
		exit();
	} else {
		header('Location: install.php');
		exit();
	}
?>
