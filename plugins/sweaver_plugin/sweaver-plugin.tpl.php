<?php
// $Id$

/**
 * @file
 * Themer editor form.
 */
?>

<div id="sweaver">

  <!-- tabs -->
  <div id="sweaver-tabs" class="clear-block">
    <div class="close"><?php print '<a href="javascript:;">x</a>'; ?></div>
    <?php
    $active = 'active-tab';
    foreach ($tabs as $key => $tab): ?>
      <div id="tab-<?php print $key; ?>" class="tab <?php print $active; ?> <?php print $key; ?>">
        <a href="javascript:;"><?php print $tab['#tab_name']; ?></a>
      </div>
    <?php
    $active = '';
    endforeach; ?>
  </div>

  <div id="sweaver-middle" class="clear-block">
     <?php
     $i = 0;
     foreach ($tabs_data as $key => $tab_data):
     ?>
     <!-- <?php print $key; ?> -->
      <div id="<?php print $key;?>" <?php ($i != 0) ? print 'style="display:none"' : '' ?>>
       <div class="sweaver-header"><?php print $tab_data['#tab_description']; ?></div>
       <div class="sweaver-content"><?php print $tab_data['content']; ?></div>
      </div>
    <?php
      $i++;
      endforeach;
    ?>
  </div>

  <?php print $rest_of_form; ?>
</div>
