<?php
// $Id$
/**
* @file
* Administration page callbacks for the infomenu module.
*/
/**
* Form builder. Configure infomenu.
*
* @ingroup forms
* @see system_settings_form().
*/

/**
 * Administration page of infomenu, shows a list of links to module options
 */
function infomenu() {
  //igual que system_admin_menu_block_page
  $item = menu_get_item();
  $content = system_admin_menu_block($item);
  $output = theme('admin_block_content', $content);
  return $output;
}

/**
  * Function to show the list of infomenu blocks
  */
function infomenu_admin() {

  $header = array(t("Title"), t("Menu"), t("Region"), t("Operations"));
  $result = pager_query("SELECT imid, text, menu_name FROM {infomenu_block}", 30, 1);
  $def_theme = variable_get('theme_default', 'garland');
  while($u = db_fetch_object($result)){
    $region = db_result(db_query("SELECT region FROM {blocks} WHERE delta = '%s' AND theme = '%s'", 'infomenu-'.$u->imid, $def_theme));
    $edit = l(t('edit'), "admin/infomenu/block/edit/$u->imid") .' | ';
	$delete = (l(t('delete'), "admin/infomenu/block/delete/$u->imid") .' | ');
	//$destination para que retorne a blocks del modulo
	$destination = "destination=admin/infomenu/block";
	$configure = l(t('configure'), "admin/build/block/configure/infomenu/infomenu-$u->imid", array("query" => $destination));

    $rows[] = array(
      array("data" => $u->text),
      array("data" => $u->menu_name, "align" => "left"),
      array("data" => $region, "align" => "center"),
      array("data" => $edit . $delete . $configure)
    );
  }

  $pager = theme("pager", NULL, 30, 1);
  if (!empty($pager)) {
    $rows[] = array(array("data" => $pager, "colspan" => "4"));
  }

  $output = theme('table', $header, $rows);

  return $output;
}

/**
 * Load an infomenu block from db
 * @param $imid
 *   id of infomenu block
 * @return
 *   an array with the fields of an specific infomenu block;
 */
function infomenu_load_block($imid) {
  $obj = db_fetch_array(db_query("SELECT * FROM {infomenu_block} WHERE imid=%d", $imid));
  return $obj;
}

/**
 * Block's Edit form
 *
 * @param $imid
 *   id of infomenu block to edit
 * @return
 *   associative array $form of $imid infomenu block
 *
 * @ingroup forms
 * @see infomenu_block_form_submit()
 * @see infomenu_block_form_validate()
 */
function infomenu_block_form( &$form_state,$imid = 0) {

  $block = infomenu_load_block($imid);
  $item  = db_query("SELECT * FROM {infomenu_block_views}");
  $types = array();

  while ($current = db_fetch_array($item)) {
  	$types[$current['type']] = $current['name'];
  }

  $form['imid'] = array(
    '#type' => 'hidden',
    '#value' => $imid,
  );

  $form['description'] = array(
    '#type' => 'textfield',
    '#title' => t('Name'),
    '#default_value' => $block ? $block['text'] : '',
    '#maxlength' => 100,
    '#description' => t('Enter the name of the block'),
    '#required' => TRUE,
  );

  $menus = menu_get_menus(true);

  $form['menu'] = array(
    '#type' => 'select',
    '#title' => t('Menu'),
    '#options' => $menus,
    '#default_value'=> $block['menu_name'],
  );
  $form['type'] = array(
    '#type' => 'select',
    '#title' => t('Type'),
    '#options' => $types,
    '#default_value' =>$block['type'],
  );

  //en que region ubicar el bloque
  $block_regions = infoutilities_regions() + array(BLOCK_REGION_NONE => '<'. t('none') .'>');
  $def_theme = variable_get('theme_default', 'garland');
  $form['def_theme'] = array(
    '#type' => 'hidden',
    '#value' => $def_theme,
  );
  if ($imid) {
    //region on default theme
	$region = db_result(db_query("SELECT region FROM {blocks} WHERE delta = '%s' AND theme = '%s'", 'infomenu-'.$imid, $def_theme));
  }

  $form['region'] = array(
    '#type' => 'select',
    '#title' => t('Region'),
    '#description' => t('Assign a block to a region.'),
    '#default_value' => $region ? $region : BLOCK_REGION_NONE,
    '#options' => $block_regions,
    '#required' => TRUE,
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Submit'),
  );
  return $form;
}

/**
 * Validate function for infomenu_block_form.
 */
function infomenu_block_form_validate($form, &$form_state)
{
   if($form_state['values']['imid']) {
    $menus = menu_get_menus(true);
    if (!$menus[$form_state['values']['menu']]){
      form_set_error('menu',t('The menu is not valid.'),false);
    }

    $item = db_query("SELECT * FROM {infomenu_block_views}");
    $types = array();
    while($current = db_fetch_array($item)) {
      $types[$current['type']] = $current['name'];
    }

    if (!$types[$form_state['values']['type']]){
      form_set_error('type',t('The view is not valid.'),false);
    }
  }
  else {
    form_set_error(NULL,t('Can not update.'),false);
  }
}

/**
 * Submit function for infomenu_block_form.
 */
function infomenu_block_form_submit($form, &$form_state) {

  $status = $form_state['values']['region'] != BLOCK_REGION_NONE;
  $region = $status ? $form_state['values']['region'] : '';
  $def_theme = $form_state['values']['def_theme'];

  if ($form_state['values']['imid']) {
    db_query("UPDATE {infomenu_block} SET text = '%s', type = '%s', menu_name = '%s', visible= '%s' WHERE imid = %d", $form_state['values']['description'], $form_state['values']['type'],$form_state['values']['menu'],'ok', $form_state['values']['imid']);
    //se actualiza title, region y status en blocks
    db_query("UPDATE {blocks} SET region='%s', status = %d WHERE delta = '%s' AND theme='%s'", $region, $status, 'infomenu-'.$form_state['values']['imid'], $def_theme);
    //guardar en log de drupal
    watchdog('block', "Updated '%module' block: '%title'.", array('%module' => 'Infomenu', '%title' => $form_state['values']['description']));
  }
  else {
	db_query("INSERT INTO {infomenu_block} (text, type,menu_name,visible) VALUES ('%s', '%s', '%s', '%s')", $form_state['values']['description'], $form_state['values']['type'], $form_state['values']['menu'],'ok');
    $imid = db_last_insert_id('infomenu_block', 'imid');
    //inserta con title, region, status y theme en blocks
	db_query("INSERT INTO {blocks} (module, delta, theme, status, weight, region) VALUES ('%s', '%s', '%s', %d, %d, '%s')", 'infomenu', 'infomenu-'.$imid, $def_theme, $status, -10, $region);
    //guardar en log de drupal
    watchdog('block', "Added '%module' block: '%title'.", array('%module' => 'Infomenu', '%title' => $form_state['values']['description']));
  }

  cache_clear_all();
  drupal_set_message("The block has been updated.");
  $form_state['redirect'] = 'admin/infomenu/block';
}

/**
 * Confirm function to delete a infomenu block.
 */
function infomenu_confirm_delete_form(&$form_state, $imid) {

  $form['block'] = array('#type' => 'value', '#value' => $imid);
  $result = db_fetch_object(db_query("SELECT text FROM {infomenu_block} WHERE imid=%d", $imid));
  $form['title'] = array('#type' => 'value', '#value' => $result->text);

  return confirm_form(
    $form,
    t('Are you sure you want to delete %text?', array('%text' => $result->text)),
    $_GET['destination'] ? $_GET['destination'] : 'admin/infomenu/block',
    t('This action cannot be undone.'),
    t('Delete'),
    t('Cancel')
  );
}

/**
 * Submit deletion of an infomenu block and return to infomenu administration page
 */
function infomenu_confirm_delete_form_submit($form, &$form_state) {
  if ($form_state['values']['confirm']) {
    infomenu_delete_block($form_state['values']['block']);
    //guardar en log de drupal
    watchdog('block', "Deleted '%module' block: '%title'.", array('%module' => 'Infomenu', '%title' => $form_state['values']['title']));
  }
  $form_state['redirect'] = 'admin/infomenu/block';
}
/**
 * Delete a block from db and return to infomenu administration page
 *
 * @param $imid
 *   id of the block to delete
 */
function infomenu_delete_block($imid) {
  db_query("DELETE FROM {infomenu_block} WHERE imid=%d", $imid);
  db_query("DELETE FROM {blocks} WHERE module = 'infomenu' AND delta = 'infomenu-%d'", $imid);
  drupal_set_message(t('The block has been deleted.'));
  cache_clear_all();
  $form_state['redirect'] = 'admin/infomenu/block';
}

/**
 * Display a menu
 * @param $menuitem_id
 *   id of menu item or a menu name
 * @return
 *   the output of the tree or subtree;
 */
function infomenu_display($menuitem_id = 'navigation'){

    if (is_numeric($menuitem_id)){
  	  $subtree = infomenu_sub_structure(infomenu_subtree($menuitem_id));
	  $subtree = theme('infomenu_sub_content', $subtree);
	  return $subtree;
	}
	$out = infomenu_structure(menu_tree_all_data($menuitem_id));
	$out = theme('infomenu_content', $out);
    return $out;

}

/**
 * Function to get a menu subtree
 * @param $mlid
 *   id of menu item
 * @return
 *   an associative array with a menu subtree;
 */
function infomenu_subtree($mlid) {
  if (!$subtree_link = menu_link_load($mlid)){
    return false;
  }
  else {
    return infomenu_getmenusby_mlid($mlid,$subtree_link);
  }
}

/**
 * Recursive function to build a subtree of menu items
 * @param $mlid
 *   id of menu item
 *   $mlid_link
 *   menu item
 * @return
 *   an associative array with a menu subtree;
 */
function infomenu_getmenusby_mlid($mlid, $mlid_link){

  $items = array();
  $count = 0;
  $items['link'] = $mlid_link;
  $items['below'] = array();

  $childs = infomenu_menu_link_load_byparent($mlid);
  if ($childs) {
	foreach($childs as $child){
  	  $items['below'][] = infomenu_getmenusby_mlid($child['mlid'], $child);
	}
  }
  return $items;
}

/**
 *Function to get the menu items with the same parent
 * @param $plid
 *   id of a menu item
 * @return
 *   A associative array of one level;
 */
function infomenu_menu_link_load_byparent($plid) {

  $items = array();
  if (is_numeric($plid) && $q = db_query("SELECT m.*, ml.* FROM {menu_links} ml LEFT JOIN {menu_router} m ON m.path = ml.router_path WHERE ml.plid = %d", $plid)){
    while ($it = db_fetch_array($q)){
      _menu_link_translate($it);
	  $items[] = $it;
    }
    return $items;
  }
  return false;
}


/**
 *Function to get the image path of a menu item
 * @param $mlid
 *   id of a menu item
 * @return
 *   A image path;
 */
function infomenu_aditional_info($mlid){
  global $base_path;
  $image_path = '';
  $abstract = '';
  $item = db_query("SELECT image_path, abstract FROM {infomenu_aditional_info} WHERE mlid = %d", $mlid);
  while ($m = db_fetch_array($item)){
	$image_path = ($m['image_path']?$base_path.$m['image_path']:'');
	$abstract =  $m['abstract'];
  }
  return array('image_path' => $image_path, 'abstract' => $abstract);
}

/**
 *Function to get three levels of a menu
 */
function infomenu_structure($tree){
  $items = array();

  $result = array();
  foreach ($tree as $data) {
    if (!$data['link']['hidden']) {
      $items [] = $data;
    }
  }

  $num_items = count($items);
  foreach ($items as $i => $data) {
    $image_path = '';
    //searching the associated image
	if($data ['link']['mlid']){
	  $info = infomenu_aditional_info($data['link']['mlid']);
 	  $image_path = $info['image_path'];
 	  $abstract = $info['abstract'];
	}

	$l = infomenu_item_menufy($data, '', '', '');

	$result[$i] = array();
	$result[$i]['link'] = $l;
	$result[$i]['image_path'] = $image_path;
	$result[$i]['abstract'] = $abstract;

    if ($data['below']) {
      $result[$i]['below'] = array();

      $items2level = array();
	  foreach ($data['below'] as $data2level) {
        if (!$data2level['link']['hidden']) {
          $items2level[] = $data2level;
	    }
	  }
	  foreach ($items2level as $j => $d) {
	    $image_path = '';
	    if($d['link']['mlid']){
	    $info = infomenu_aditional_info($d['link']['mlid']);
 	    $image_path = $info['image_path'];
 	    $abstract = $info['abstract'];
		}

		$l = '';
		$l = infomenu_item_menufy($d, '', '', '');
        $result[$i]['below'][$j] = array();
		$result[$i]['below'][$j]['link'] = $l;
		$result[$i]['below'][$j]['image_path'] = $image_path;
		$result[$i]['below'][$j]['abstract'] = $abstract;

		if ($d['below'])	{
		  $result[$i]['below'][$j]['below'] = array();
		  $items3level = array();
		  foreach ($d['below'] as $data3level) {
            if (!$data3level['link']['hidden']) {
              $items3level[] = $data3level;
	        }
	      }

	      foreach ($items3level as $p => $d3) {
	        $image_path = '';
	        if($d3['link']['mlid']){
	        $info = infomenu_aditional_info($d3['link']['mlid']);
 	    	$image_path = $info['image_path'];
 	    	$abstract = $info['abstract'];
		    }
  		    $l = '';
		    $l = infomenu_item_menufy($d3, '', '', '');
		    $result[$i]['below'][$j]['below'][$p] = array();
		    $result[$i]['below'][$j]['below'][$p]['link'] = $l;
		    $result[$i]['below'][$j]['below'][$p]['image_path'] = $image_path;
		    $result[$i]['below'][$j]['below'][$p]['abstract'] = $abstract;
		  }
	    }
      }
    }
  }

  return $result;
}

/**
 *Function to get three levels of a sub menu
 */
function infomenu_sub_structure($tree){


  $result=array();
  $image_path='';
  if($tree['link']['mlid']){
  	$info = infomenu_aditional_info($tree['link']['mlid']);
 	$image_path = $info['image_path'];
 	$abstract = $info['abstract'];
  }

  $l=infomenu_item_menufy($tree,'','','');
  $result['image_path']=$image_path;
  $result['link']=$l;
  $result['description']= $tree['link']['options']['attributes']['title'];
  $result['abstract'] = $abstract;

  if ($tree['below']) {
    $result['below']=array();
    $items2level=array();
	foreach ($tree['below'] as $data2level) {
      $items2level[] = $data2level;
	}
	foreach ($items2level as $k => $d) {
	    $image_path='';
    	if($d['link']['mlid']){
    	  $info = infomenu_aditional_info($d['link']['mlid']);
 	      $image_path = $info['image_path'];
 	      $abstract = $info['abstract'];
		}
        $l='';
		$l=infomenu_item_menufy($d,$image_tag,'','');

		$result['below'][$k]=array();
		$result['below'][$k]['image_path']=$image_path;
		$result['below'][$k]['link']=$l;
  		$result['below'][$k]['description']=$d['link']['options']['attributes']['title'];
  		$result['below'][$k]['abstract'] = $abstract;

		if($d['below'])	{
		  $result['below'][$k]['below']=array();
		  $items3level=array();
		  foreach ($d['below'] as  $data3level) {
            $items3level[] = $data3level;
	      }

	      foreach ($items3level as $k3 => $d3) {
	        $image_path='';
  		    $l='';
		    $l=infomenu_item_menufy($d3,$image_path,'','');
		    $result['below'][$k]['below'][$k3]=array();
		    $result['below'][$k]['below'][$k3]['link']=$l;
		  }
		}
	  }
    }
  return $result;

}

/*
 *
Customize a link
*/
function infomenu_item_menufy($data,$image_tag='', $initial_tag, $final_tag) {

  if($data['link']['href'])	{
    $atributtes = array();
	if (menu_path_is_external($data['link']['href'])) {
      $atributtes = array('attributes' => array('target' => '_blank'));
	}
	$link = l($data['link']['title'], $data['link']['href'], $atributtes);
	$result = $initial_tag . $image_tag . $link.$final_tag;
	return $result;
  }
  return '';
}