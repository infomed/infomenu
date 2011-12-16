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
<div class="dinamic_infomenu">
<ul>
<?php foreach ($content as $key => $field) : ?>



<li>
<?php if($field['image_path']!='')print '<img src="'.$field['image_path'].'"  width="25" height="25" align="middle">'; ?>
<?php print $field['link']; ?>




<?php if ($field['below']) : ?>

<ul>
<?php foreach ($field['below'] as $key2 => $field2) : ?>


<li>
<?php if($field2['image_path']!='')print '<img src="'.$field2['image_path'].'"  width="25" height="25" align="middle">'; ?>
<?php print $field2['link']; ?>



<?php if ($field2['below']) : ?>
<ul>
<?php foreach ($field2['below'] as $key3 => $field3) : ?>
<li>
<?php print $field3['link']; ?>
</li>


<?php endforeach; ?>
</ul>

<?php endif; ?>
</li>


<?php endforeach; ?>
</ul>

<?php endif; ?>



</li>

<?php endforeach; ?>
</ul>
</div>