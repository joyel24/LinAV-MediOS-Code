<?
	require("include/affBlock.class");
	require("include/affMenu.class");
	require("include/enteteHtml.php");

	entete();

	//echo "<center>";
	/*echo "<IMG SRC='/images/avos_top.jpg' WIDTH=180  border=0>"
		. "<IMG SRC='/images/avos_bottom.jpg' WIDTH=180  border=0>";*/
	//echo "<img src='/images/linav.jpg' border=0 alt='Linux on Av320,Av340,Av380'>\n";
	//echo "</center>\n";
	//echo "<br><font size=-2><div align='right'>logo by midk</div></font>\n";
	//echo "<table height='10' border='0' cellpadding='0' cellspacing='0'>\n";
	//echo "<tr><td></td></tr>\n";
	//echo "</table>\n";

	$test = new affMenu();

	$test->debItem("Menu");
	$test->addLien("Home","index.php?page=home.php","_parent");
	$test->addLien("Files","index.php?page=files.php","_parent");
	$test->addLien("Doc","index.php?page=doc.php","_parent");
	$test->addLien("HowTo","index.php?page=howto.php","_parent");
	$test->addLien("Links","index.php?page=links.php","_parent");
	$test->addLien("Developers","index.php?page=dev.php","_parent");
	$test->finItem();
	
//ob_start();
//include("../donations.html");
//$str=ob_get_contents();
//ob_end_clean();
//echo $str;
	$test->debItem("Sourceforge");
	$test->addLien("<img src='images/sflogo.png'"
		. " border=0 alt='Sourceforge logo'>",
		"http://sourceforge.net/projects/linav",
		"_blank");
	/*$test->addLien("<center><img src='http://images.sourceforge.net/images/project-support.jpg' width='88' height='32' border='0' alt='Support This Project' /></center>",
		"http://sourceforge.net/donate/index.php?group_id=108103",
		"_blank");*/

	$test->addLien("<img src='images/cvs.png' border='0'>",
			"http://cvs.sourceforge.net/viewcvs.py/linav/",
			"_blank");
	include("paypal.inc");
	$test->finItem();

	$test->debItem("Yahoo");
	$test->addLien("<img src='images/yGroup.gif' border='0'>",
                       "http://groups.yahoo.com/group/linav/",
                       "_blank");
	$test->finItem();

	
	
include_once("prive/phpsitestats/inc.conf.php");
include_once("prive/phpsitestats/inc.func.php");
echo "<br><center>\n";
fl_show_days_avg("/home/groups/l/li/linav/htdocs/prive/phpsitestats/data.log");
echo "</center>\n";
?>
</body>
</html>
