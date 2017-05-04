<?php
	/* Thanks w3schools and linkedcabin */
	/* For the search:
	Select * from bots where lower(Concat(bot_id, '', bot_key, '', bot_version)) like lower("%1.2%");
	Add more columns but yeah
	also its case insentive
	*/

	require_once '../include/main.php';
	session_start();

	if (!isset($_SESSION['username']))
	{
		header('Location: index.php');
		exit;
	}

	// Get the IP and put it in the user database
	$updateinfo = $conn->prepare("UPDATE users SET lastip = :ip, lastseen = :seen WHERE `username` = :user");
	$updateinfo->execute(array(
		':ip'=>$_SERVER['REMOTE_ADDR'],
		':seen'=>$TIME,
		':user'=>$_SESSION['username']
	));

	if(isset($_GET["p"]))
	{
		$page = htmlspecialchars($_GET["p"]);
		if ($page == "bots")
			$pagetype = 1;
		else if ($page == "cmdlog")
			$pagetype = 2;
		else if ($page == "users")
			$pagetype = 3;
		else if ($page == "logs")
			$pagetype = 4;
		else if ($page == 'cmd')
			$pagetype = 5;
		else if ($page == "logout")
		{
			session_destroy();
			header("location: index.php");
			exit;
		}
		else
		{
			$pagetype = 1;
		}
	}
	else
		$pagetype = 1;
?>

<!DOCTYPE html>
<html>
	<head>
		<title>Artemis - Dashboard</title>
		<link rel="stylesheet" type="text/css" href="../css/main.css">
	</head>
	<body>
		<!-- Side navigation menu -->
		<div id="sidenav">
			<div align="center">
				<div id="topofnav">Welcome: <b><?php echo($_SESSION['username']) ?></b></div>
			</div>
			<ul>
				<li><a href="?p=bots">Bots</a></li>
				<li><a href="?p=cmd">Command</a></li>
				<li><a href="?p=stats">Command Exec Logs</a></li>
				<li><a href="?p=users">Users</a></li>
				<li><a href="?p=logs">Logs</a></li>
				<li><a href="?p=logout">Logout</a></li>

				<li><span><b>ChangeLog:</b></span></li>
				<li><span><?php echo($CHANGELOG) ?></span></li>
			</ul>
		</div>

		<!-- Page -->
		<div id="main">
			<div style="margin-left:12%; padding:1px 16px; height:1000px;">
			<?php
				if ($pagetype == 1) // Bots
					require_once "pages/bots.php";
				else if ($pagetype == 2) // Command Exec Log
					require_once "pages/cmdlog.php";
				else if ($pagetype == 3) // Users
					require_once "pages/users.php";
				else if ($pagetype == 4) // Logs
					require_once "pages/log.php";
				else if ($pagetype == 5) // Cmd
					require_once "pages/cmd.php";
			?>
			</div>
		</div>
	</body>
</html>
