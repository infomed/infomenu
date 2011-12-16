<?php

/**
 * @file infomenu-item-first-level.tpl.php
 * Default theme implementation to present an first level item of infomenu.
 *
 * Available variables:
 * - $content:  a content array
 *
 *
 * @see
 */
?>

<?php foreach ($content as $key => $field) : ?>

<div class="collection">
<div class="first_level">
<div>
<?php if($field['image_path']!='') print '<img src="'.$field['image_path'].'"  width="15" height="15" >'; ?>
<?php print $field['link']; ?>
</div>
</div>
 <div class="collection_inner">
<?php if ($field['below']) : ?>
<?php foreach ($field['below'] as $key2 => $field2) : ?>

<div class="second_level">
<div>
<?php if($field2['image_path']!='')print '<img src="'.$field2['image_path'].'"  width="15" height="15" >'; ?>
<?php print $field2['link']; ?>
</div>
</div>
 <div class="spanmenu">
<?php if ($field2['below']) : ?>
<?php foreach ($field2['below'] as $key3 => $field3) : ?>
<span>
<?php print $field3['link']; ?>
</span>

<?php endforeach; ?>

<?php endif; ?>
</div>


<?php endforeach; ?>

<?php endif; ?>
</div>


</div>


<?php endforeach; ?>