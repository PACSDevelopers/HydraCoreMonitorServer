'use strict';

function submitForm() {
  var $alertBox = $('#alertBox');
  $alertBox.slideUp().html(bootstrapAlert('info', 'Sending request for database creation.')).slideDown();

  var submitThis = true;
  var inputs = [
    'databaseTitle',
    'databaseIP',
    'databaseUsername',
    'databasePassword',
    'databaseBackupType',
    'databaseBackupInterval'
  ];

  var data = {};
  inputs.forEach(function(value) {
    switch (checkFormElement('#' + value)) {
      case true:
        data[value] = getInputValue('#' + value);
        break;

      case false:
        submitThis = false;
        break;
      default:
        // Skip value
        break;
    }
  });

  if (submitThis) {
    $.ajax({
      type: "POST",
      url: '/ajax/databases/database/processNewDatabase',
      data: {
        data: data
      }
    })
      .done(function(response) {
        if (typeof(response['status']) != 'undefined') {
          if (typeof(response['databaseID']) != 'undefined') {
            if (response['databaseID']) {
              window.location.href = '/databases/' + response['databaseID'];
            } else {
              // Tell user error
              $alertBox.html(bootstrapAlert('danger', 'Something went wrong, please try again.')).slideDown();
            }
          }
        } else {
            $alertBox.html(bootstrapAlert('warning', 'The details you entered are not valid, please try again.')).slideDown();
        }
      })
      .fail(function() {
        // Tell user error
        $alertBox.html(bootstrapAlert('danger', 'Something went wrong, please try again.')).slideDown();
      });
  } else {
    $alertBox.html(bootstrapAlert('warning', 'The details you entered are not valid, please try again.')).slideDown();
  }
}

function updateForm() {
  var $alertBox = $('#alertBox');
  $alertBox.slideUp().html(bootstrapAlert('info', 'Sending request for database update.')).slideDown();

  var submitThis = false;
  var inputs = [
      'databaseTitle',
      'databaseIP',
      'databaseUsername',
      'databasePassword',
      'databaseBackupType',
      'databaseBackupInterval'
  ];

  var data = {};
  inputs.forEach(function(value) {
    switch (checkFormElement('#' + value)) {
      case true:
        data[value] = getInputValue('#' + value);
        submitThis = true;
        break;
      default:
        // Skip value
        break;
    }
  });

  if (submitThis) {
    // Append database ID
    data['databaseID'] = $('#databaseID').val();
      
    $.ajax({
      type: "POST",
      url: '/ajax/databases/database/processUpdateDatabase',
      data: {
        data: data
      }
    })
      .done(function(response) {
        if (typeof(response.status) != 'undefined') {
          $alertBox.html(bootstrapAlert('success', 'Database successfully edited.')).slideDown();
        } else {
            $alertBox.html(bootstrapAlert('warning', 'The details you entered are not valid, please try again.')).slideDown();
        }
      })
      .fail(function() {
        // Tell user error
        $alertBox.html(bootstrapAlert('danger', 'Something went wrong, please try again.')).slideDown();
      });
  } else {
    $alertBox.html(bootstrapAlert('warning', 'The details you entered are not valid, please try again.')).slideDown();
  }
}

function deleteDatabase() {
    var $alertBox = $('#alertBox');
    $alertBox.slideUp().html(bootstrapAlert('info', 'Sending request for database deletion.')).slideDown();
    var data = {'databaseStatus': 0};

    // Append database ID
    data['databaseID'] = $('#databaseID').val();

    $.ajax({
        type: "POST",
        url: '/ajax/databases/database/processUpdateDatabase',
        data: {
            data: data
        }
    })
        .done(function(response) {
            if (typeof(response.status) != 'undefined') {
                $alertBox.html(bootstrapAlert('success', 'Database successfully deleted.')).slideDown();
            }
        })
        .fail(function() {
            // Tell user error
            $alertBox.html(bootstrapAlert('danger', 'Something went wrong, please try again.')).slideDown();
        });
}

function backupDatabase() {
    var $alertBox = $('#alertBox');
    $alertBox.slideUp().html(bootstrapAlert('info', 'Sending request for database backup.')).slideDown();
    var data = {'databaseID': $('#databaseID').val()};

    $.ajax({
        type: "POST",
        url: '/ajax/databases/database/processBackupDatabase',
        data: {
            data: data
        }
    })
        .done(function(response) {
            if (typeof(response.status) != 'undefined') {
                switch(response.status) {
                    case 1:
                        $alertBox.html(bootstrapAlert('success', 'Database backup scheduled.')).slideDown();
                        break;
                    
                    case 2:
                        $alertBox.html(bootstrapAlert('info', 'Database is already being backed up.')).slideDown();
                        break;
                    
                    default:
                        $alertBox.html(bootstrapAlert('danger', 'Something went wrong, please try again.')).slideDown();
                        break;
                }
                
            } else {
                // Tell user error
                $alertBox.html(bootstrapAlert('danger', 'Something went wrong, please try again.')).slideDown();
            }
        })
        .fail(function() {
            // Tell user error
            $alertBox.html(bootstrapAlert('danger', 'Something went wrong, please try again.')).slideDown();
        });
}

function getArchiveFromVault(id) {
    var $alertBox = $('#alertBox');
    $alertBox.slideUp().html(bootstrapAlert('info', 'Sending request for backup from vault.')).slideDown();
    var data = {'id': id};

    $.ajax({
        type: "POST",
        url: '/ajax/databases/database/processArchiveRequest',
        data: {
            data: data
        }
    })
        .done(function(response) {
            if (typeof(response.status) != 'undefined') {
                if(response.status == 1) {
                    $alertBox.html(bootstrapAlert('success', 'Request sent to vault successfully.')).slideDown();
                } else {
                    $alertBox.html(bootstrapAlert('info', 'Request already active.')).slideDown();
                }
            } else {
                $alertBox.html(bootstrapAlert('danger', 'Something went wrong, please try again.')).slideDown();
            }
        })
        .fail(function() {
            // Tell user error
            $alertBox.html(bootstrapAlert('danger', 'Something went wrong, please try again.')).slideDown();
        });
}

function deleteBackup(id) {
    var $alertBox = $('#alertBox');
    $alertBox.slideUp().html(bootstrapAlert('info', 'Sending request for backup deletion.')).slideDown();
    var data = {'id': id};

    $.ajax({
        type: "POST",
        url: '/ajax/databases/database/processDeleteRequest',
        data: {
            data: data
        }
    })
        .done(function(response) {
            if (typeof(response.status) != 'undefined') {
                if(response.status == 1) {
                    $alertBox.html(bootstrapAlert('success', 'Backup deleted successfully.')).slideDown();
                }
            } else {
                $alertBox.html(bootstrapAlert('danger', 'Something went wrong, please try again.')).slideDown();
            }
        })
        .fail(function() {
            // Tell user error
            $alertBox.html(bootstrapAlert('danger', 'Something went wrong, please try again.')).slideDown();
        });
}

function deleteArchiveFromVault(id) {
    var $alertBox = $('#alertBox');
    $alertBox.slideUp().html(bootstrapAlert('info', 'Sending request for backup deletion from vault.')).slideDown();
    var data = {'id': id};

    $.ajax({
        type: "POST",
        url: '/ajax/databases/database/processDeleteArchiveRequest',
        data: {
            data: data
        }
    })
        .done(function(response) {
            if (typeof(response.status) != 'undefined') {
                if(response.status == 1) {
                    $alertBox.html(bootstrapAlert('success', 'Request sent to vault successfully.')).slideDown();
                }
            } else {
                $alertBox.html(bootstrapAlert('danger', 'Something went wrong, please try again.')).slideDown();
            }
        })
        .fail(function() {
            // Tell user error
            $alertBox.html(bootstrapAlert('danger', 'Something went wrong, please try again.')).slideDown();
        });
}