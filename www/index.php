<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1">
<meta name="Author" content="">
<meta name="Keywords" content="av3xx archos linav">
<title>Linux on Av3xx(Archos): linav (Av320-Av340-Av380)</title>
<meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1">
</head>
<body>

<table width='100%' height='140' cellpadding='0' cellspacing='0' border='0'>
<TR>
<TD width='358'>
<IMG SRC='/images/linav-left.jpg' border='0'>
</TD>
<td style="background-image: url('/images/linav-center.jpg');">
&nbsp;
</td>
<td width='401'>
<IMG SRC='/images/linav-right.jpg' border='0'>
</td>
</TR></table>
<br><font size=-2><div align='right'>logo by McLeodz & Midk</div></font>

<table width="100%">
<TR><TD valign="top">
<?
include("menu.php");
include_once("prive/phpsitestats/inc.conf.php");
include_once("prive/phpsitestats/inc.func.php");
//_visits_log("/home/groups/l/li/linav/htdocs/prive/phpsitestats/data.log");
?>
</TD>
<TD valign="top">
<?
if(!isset($page))
{
	fl_visits_log("/home/groups/l/li/linav/htdocs/prive/phpsitestats/data.log");
	$page="home.php";
}
include($page);
?>
</TD></tr>
<tr><TD colspan="2">
<?
include("bottom.php");
?>
</TD></tr>
</table>
</body>
</html>
