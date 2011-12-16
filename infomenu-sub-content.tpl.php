<?php

/**
 * @file infomenu-item-first-level.tpl.php
 * Default theme implementation to present an first level item of infomenu.
 *
 * Available variables:
 * - $content:  a content array in a clicked infomenu link
 *
 *
 * @see
 */
?>



<div class="subcontent">
<div class="first_level">
<div>
<?php if($content['image_path']!='')print '<img src="'.$content['image_path'].'"  width="15" height="15" align="middle">'; ?>
<?php print $content['link']; ?>
<?php if(!$content['below']) : ?>
<?php print '<p class="description">'.$content['description'].'</p>'; ?>
<?php endif; ?>
</div>
</div>

<?php if ($content['below']) : ?>
<?php foreach ($content['below'] as $key2 => $field2) : ?>

<div >
<div class="second_level">
<?php if($field2['image_path']!='')print '<img src="'.$field2['image_path'].'"  width="15" height="15" align="middle">'; ?>
<?php print $field2['link']; ?>
</div>
<?php print '<p class="description">'.$field2['abstract'].'</p>'; ?>
</div>

<?php endforeach; ?>

<?php endif; ?>



</div>
