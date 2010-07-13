<?php
// $Id$

/**
 * Images plugin.
 */
class sweaver_plugin_images extends sweaver_plugin {

  /**
   * Menu registry.
   */
  function menu_registry(&$weight) {

    $items = array();
    $base = array(
      'load arguments' => array('%map'),
      'access arguments' => array('configure sweaver'),
      'file' => 'plugins/sweaver_plugin_images/sweaver_plugin_images.admin.inc',
    );

    // Images.
    $items['admin/settings/sweaver/images'] = $base + array(
      'title' => 'Images',
      'page callback' => 'sweaver_images_list',
      'type' => MENU_LOCAL_TASK,
      'weight' => $weight++,
    );
    $items['admin/settings/sweaver/images/add'] = $base + array(
      'title' => 'Add new image',
      'page callback' => 'drupal_get_form',
      'page arguments' => array('sweaver_image_form'),
      'type' => MENU_CALLBACK,
    );
    $items['admin/settings/sweaver/images/edit'] = $base + array(
      'title' => 'Edit image',
      'page callback' => 'drupal_get_form',
      'page arguments' => array('sweaver_image_form'),
      'type' => MENU_CALLBACK,
    );
    $items['admin/settings/sweaver/images/delete'] = $base + array(
      'title' => 'Add new image',
      'page callback' => 'drupal_get_form',
      'page arguments' => array('sweaver_image_delete'),
      'type' => MENU_CALLBACK,
    );

    return $items;
  }

  /**
   * Frontend form: add image form.
   */
  function frontend_form() {

    $form['image_form']['image_description'] = array(
      '#type' => 'textfield',
      '#title' => t('Description'),
      '#description' => t('Description of the image used in the select box for files.')
    );
    $form['image_form']['image_image'] = array(
      '#type' => 'file',
      '#title' => t('Image'),
      '#description' => t('Upload images which you can use as background images. Extensions are limited to jpg, jpeg, png and gif.<br />Note: there is no scaling on the images on upload.'),
    );
    $form['image_form']['image_submit'] = array(
      '#type' => 'submit',
      '#value' => t('Save image'),
    );

    return $form;
  }

  /**
   * Frontend form submit handler.
   */
  function frontend_form_submit($form, &$form_state) {
    $_SESSION['sweaver_editor_messages']['image_save'] = '';

    if ($form_state['clicked_button']['#value'] == t('Save image')) {

      $validators = array(
        'file_validate_is_image' => array(),
      );

      $path = file_create_path('sweaver');
      file_check_directory($path, FILE_CREATE_DIRECTORY);
      $image = file_save_upload('image_image', $validators, file_directory_path() .'/sweaver');

      if ($image && !empty($form_state['values']['image_description'])) {
        $image->description = $form_state['values']['image_description'];
        $image_parts = explode('.', $image->filepath);
        $extension = array_pop($image_parts);
        $new_image_name = file_directory_path() .'/sweaver/sweaver_image_'. $image->fid .'.'. $extension;
        rename($image->filepath, $new_image_name);

        db_query("UPDATE {files} set filepath = '%s' WHERE fid = %d", $new_image_name, $image->fid);
        file_set_status($image, FILE_STATUS_PERMANENT);
        drupal_write_record('sweaver_image', $image);
        $_SESSION['sweaver_editor_messages']['image_save'] = t('Image @image has been uploaded.', array('@image' => t($image->description)));
      }
      else {
        $_SESSION['sweaver_editor_messages']['image_save'] = t('An error occured while uploading the image.');
      }
    }
  }
}
