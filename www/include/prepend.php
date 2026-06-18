<?
$new_include_dir = $DOCUMENT_ROOT . "/include";
//echo "$new_include_dir<br>";
set_include_path(get_include_path() . ":" . $new_include_dir);
?>
