<?php
// $Id$

/**
 * @file
 * Images adminstration functions.
 */

/**
 * sweaver images list.
 */
function sweaver_images_list() {
  $rows = array();
  $output = l(t('Add new image'), 'admin/settings/sweaver/images/add');

  $query = 'SELECT ti.fid, ti.description, f.filepath FROM {sweaver_image} ti
            INNER JOIN {files} f on f.fid = ti.fid order by description ASC';
  $result = db_query($query);
  while ($image = db_fetch_object($result)) {
    $row = array();
    $row[] = check_plain($image->description);
    $operations = l(t('View'), file_create_url($image->filepath)) .' - ';
    $operations .= l(t('Edit'), 'admin/settings/sweaver/images/edit/'. $image->fid) .' - ';
    $operations .= l(t('Delete'), 'admin/settings/sweaver/images/delete/'. $image->fid);
    $row[] = $operations;
    $rows[] = $row;
  }

  if (!empty($rows)) {

    // Output table.
    $header = array(
      t('Image'),
      t('Operations'),
    );

    $output .= theme('table', $header, $rows);
  }
  else {
    $output .= '<p>'. t('No images uploaded.') .'</p>';
  }

  return $output;
}

/**
 * New image form.
 */
function sweaver_image_form($form_state, $fid = NULL) {
  $form = array();

  $description = '';
  $check_upload = TRUE;
  $image = _sweaver_get_image($fid);
  if ($image) {
    $check_upload = FALSE;
    $description = $image->description;
    $fid = $image->fid;
  }
  else {
    $fid = NULL;
  }

  $form['#attributes'] = array('enctype' => 'multipart/form-data');
  $form['#fid'] = $fid;
  $form['#check_upload'] = $check_upload;

  $form['image_form']['description'] = array(
    '#type' => 'textfield',
    '#title' => t('Description'),
    '#required' => TRUE,
    '#default_value' => $description,
    '#description' => t('Description of the image used in the select box for files.')
  );
  if (!$fid) {
    $form['image_form']['image'] = array(
      '#type' => 'file',
      '#title' => t('Image'),
      '#description' => t('Upload images which you can use as background images. Extensions are limited to jpg, jpeg, png and gif.<br />Note: there is no scaling on the images on upload.'),
    );
  }
  $form['image_form']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save image'),
  );

  return $form;
}

/**
 * Image validate callback.
 */
function sweaver_image_form_validate(&$form, &$form_state) {
  $validators = array(
    'file_validate_is_image' => array(),
  );

  $path = file_create_path('sweaver');
  file_check_directory($path, FILE_CREATE_DIRECTORY);
  $image = file_save_upload('image', $validators, file_directory_path() .'/sweaver');
  if ($image) {
    $form_state['image'] = $image;
  }
  elseif ($form['#check_upload']) {
    form_set_error('image', t('Image is required'));
  }
}

/**
 * Image submit callback.
 */
function sweaver_image_form_submit(&$form, &$form_state) {
  $image = new stdClass;

  if (isset($form_state['image'])) {
    $image_uploaded = $form_state['image'];
    $image->fid = $image_uploaded->fid;
    $image_parts = explode('.', $image_uploaded->filepath);
    $extension = array_pop($image_parts);
    $new_image_name = file_directory_path() .'/sweaver/sweaver_image_'. $image->fid .'.'. $extension;
    rename($image_uploaded->filepath, $new_image_name);
    db_query("UPDATE {files} set filepath = '%s' WHERE fid = %d", $new_image_name, $image->fid);
    file_set_status($image_uploaded, FILE_STATUS_PERMANENT);
  }

  if (!isset($image->fid)) $image->fid = $form['#fid'];
  $update = (!$form['#check_upload']) ? array('fid') : array();
  $image->description = $form_state['values']['description'];
  drupal_write_record('sweaver_image', $image, $update);

  drupal_set_message(t('Image %description has been saved.', array('%description' => $image->description)));
  $form_state['redirect'] = 'admin/settings/sweaver/images';
}

/**
 * Image delete form.
 */
function sweaver_image_delete($form_state, $fid) {
  $image = _sweaver_get_image($fid);
  if ($image) {
    $form['#image'] = $image;
    return confirm_form($form, t('Are you sure you want to delete image %description?', array('%description' => $image->description)), 'admin/settings/sweaver/images');
  }
  else {
    drupal_set_message(t('Image not found'));
    drupal_goto('admin/settings/sweaver/images');
  }
}

/**
 * Image delete submit callback.
 */
function sweaver_image_delete_submit(&$form, &$form_state) {
  $image = $form['#image'];
  file_delete($image->filepath);
  db_query("DELETE FROM {files} where fid = %d", $image->fid);
  db_query("DELETE FROM {sweaver_image} where fid = %d", $image->fid);
  drupal_set_message(t('Image %description has been deleted.', array('%description' => $image->description)));
  $form_state['redirect'] = 'admin/settings/sweaver/images';
}

/**
 * Helper function to get image.
 * @param integer $fid The file id.
 */
function _sweaver_get_image($fid) {
  if (!empty($fid)) {
    $query = 'SELECT ti.fid, ti.description, f.filepath FROM {sweaver_image} ti
              INNER JOIN {files} f on f.fid = ti.fid WHERE f.fid = %d';
    $image = db_fetch_object(db_query($query, $fid));
    if ($image->fid) {
      return $image;
    }
  }
  return FALSE;
}
