<?
	require("include/enteteHtml.php");
	require("include/affBlock.class");
	require("include/dispItem.class");
	require("include/affPage.class");
	require("include/configTable.inc");

	entete();
	//$titre2="Home";
	//$page=new affPage($titre,$titre2);
?>
	<table width='100%'>
	<tr>
	<td >
	<center><b>
	<font size=+1>Packages have been released on <a href='http://www.sf.net/projects/linav'>Linav's SF site</a>:
 	
	<a href='http://sourceforge.net/project/showfiles.php?group_id=108103&package_id=126697&release_id=275354'>Gui2 package</a>
	&nbsp;
	<a href='http://sourceforge.net/project/showfiles.php?group_id=108103&package_id=130123&release_id=272305'>AvLo package</a>
	</font>
	</b></center>
	</td>
	</tr>


<?
/*
	$filename="output.txt";
	$handle = fopen ($filename, "r");
	$contents = fread ($handle, filesize ($filename));
	$contents=nl2br($contents);
	echo $contents
*/
?>

	</td>
	</tr>
	<tr><td height='4'></td></tr>
	<tr>
	<td>
<?
	$defaultNews=6;
	$nbNews=7;
	if(!isset($newsN))
		$newsN=$defaultNews;

	$news_list[6]["file"]="news_10.php";
        $news_list[6]["name"]="October-04";
	$news_list[5]["file"]="news_09.php";
        $news_list[5]["name"]="September-04";	
	$news_list[4]["file"]="news_08.php";
	$news_list[4]["name"]="August-04";
	$news_list[3]["file"]="news_07.php";
	$news_list[3]["name"]="July-04";
	$news_list[2]["file"]="news_06.php";
	$news_list[2]["name"]="June-04";
	$news_list[1]["file"]="news_05.php";
	$news_list[1]["name"]="May-04";
	$news_list[0]["file"]="news_04.php";
	$news_list[0]["name"]="April-04";
	
	$list="";
	for($i=0;$i<$nbNews;$i++)
		if($i==$newsN)
			$list.=$news_list[$i]["name"] . "&nbsp;&nbsp;";
		else
			$list.="<a href='index.php?page=home.php&newsN=$i'>" .$news_list[$i]["name"]."</a>&nbsp;&nbsp;";
		
	$news=new dispItem("News");
	$news->addItem("","<center><font color='green'>$list</font></center>");
	$news->addItem("","<center><font color='red' size='+1'><b>".$news_list[$newsN]["name"]."</b></font></center>");
	include($news_list[$newsN]["file"]);
	$news->endDisp();
?>
	</td></tr>
	<tr><td height='4'></td></tr>
	<tr><TD>
<?
	$test = new affMenu();
	$test->debItem("Related projects");
	echo "<table border=0 width=100%><tr>\n";
	echo "<td width='33%'><center><a class='menuMain' href='http://avos.sf.net' target='_blank'>"
		."<img src='images/avos.gif' border=0></a></center></td>\n";
	echo "<td width='33%'><center><a class='menuMain' href='http://www.sf.net/projects/garme' target='_blank'>"
		."Garme</a></center></td>\n";	
	echo "<td width='33%'><center><a class='menuMain' href='http://www.donat.org/archos/' target='_blank'>"
		."<img src='images/gmEmu.jpg' border='0'></a></center></td>\n";
	echo "</tr></table>\n";
	$test->finItem();
	$test->fin();
?>
	</td>
	</tr>
	</table>
<?
	//$page->fin();
//	finPage();
?>
