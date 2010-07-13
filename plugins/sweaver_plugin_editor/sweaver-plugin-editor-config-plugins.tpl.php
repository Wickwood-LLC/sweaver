<?php
// $Id$

/**
 * @file
 * Template file for the plugins configuration form.
 */

 $count = 0;
?>

<p><?php print t('You can enable or disable plugins on this screen and select the order of them on the frontend.'); ?></p>

<div id="plugins-configuration-form">

  <table id="plugins-configuration" class="sticky-enabled">
    <thead>
      <tr>
        <th><?php print t('Plugin'); ?></th>
        <th><?php print t('Enabled'); ?></th>
        <th><?php print t('Weight'); ?></th>
      </tr>
    </thead>

    <tbody>
    <?php foreach ($rows as $row): ?>

      <tr class="<?php print $count % 2 == 0 ? 'odd' : 'even'; ?> draggable property-row">
        <td><?php print $row->name; ?></td>
        <td><?php print $row->status; ?></td>
        <td><?php print $row->weight; ?></td>
      </tr>

    <?php endforeach; ?>
    </tbody>
  </table>

  <?php print $submit; ?>

</div>
