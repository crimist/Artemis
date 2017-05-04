<?php
	session_start();
	require_once '../include/main.php';

	if(isset($_POST['submit'])) {
		$errMsg = '';
		//username and password sent from Form
		$username = trim($_POST['username']);
		$password = trim($_POST['password']);

		if($username == '')
			$errMsg .= 'You must enter your Username<br>';

		if($password == '')
			$errMsg .= 'You must enter your Password<br>';


		if($errMsg == '') {
			$records = $conn->prepare('SELECT id, admin, username, password FROM users WHERE username = :username');
			$records->execute(array(
				':username'=>$username
			));

			$results = $records->fetch(PDO::FETCH_ASSOC);

			if(count($results) > 0 && hash('sha512', $password) == $results['password']) {
				$_SESSION['username'] = $results['username'];
				$_SESSION['admin'] = $results['admin'];
				header('location:dashboard.php');
				exit;
			}
			else {
				$errMsg .= 'Username and Password are not found<br>';
			}
		}
	}
?>


<html>
	<head>
		<title>Artemis - Login</title>
		<link rel="stylesheet" type="text/css" href="../css/main.css">
	</head>
	<body>
		<div>
			<div id="loginnav">
				<?php
					if(isset($errMsg)){
						echo '<div style="color:#FF0000;text-align:center;font-size:12px;">'.$errMsg.'</div>';
					}
				?>
				<div id="topofnav"><b>Login</b></div>
				<div style="margin:30px">
					<form action="" method="post">
						<input type="text" name="username" class="box" placeholder="Username"/><br /><br />
						<input type="password" name="password" class="box" placeholder="Password"/><br/><br />
						<input type="submit" name='submit' value="Submit" class='submit' id="nobutton"/><br />
					</form>
				</div>
			</div>
		</div>
	</body>
</html>
