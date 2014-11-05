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

function transferBackup(id, id2) {
    if(id2) {
        console.log(id, id2);
        $('#transferBackupModal').modal('hide');
        $('#transferBackupModal, .modal-backdrop').remove();
        
        var $alertBox = $('#alertBox');
        $alertBox.slideUp().html(bootstrapAlert('info', 'Sending transfer request.')).slideDown();
        
        $.ajax({
            type: "POST",
            url: '/ajax/databases/database/processBackupTransfer',
            data: {
                data: {id: id, id2: id2}
            }
        })
        .done(function(response) {
            if (typeof(response.status) != 'undefined') {
                if (response.status == 1) {
                    $alertBox.slideUp().html(bootstrapAlert('success', 'Transfer successfully scheduled.')).slideDown();
                } else {
                    $alertBox.html(bootstrapAlert('danger', 'Something went wrong, please try again.')).slideDown();
                }
            } else {
                $alertBox.html(bootstrapAlert('danger', 'Something went wrong, please try again.')).slideDown();
            }
        })
        .fail(function() {
            // Tell user error
            $alertBox.html(bootstrapAlert('danger', 'Something went wrong, please try again.')).slideDown();
        });
        
    } else {
        console.log(id);
        $.ajax({
            type: "POST",
            url: '/ajax/databases/database/processBackupTransfer',
            data: {
                data: {id: id}
            }
        })
        .done(function(response) {
            if (typeof(response.status) != 'undefined') {
                if(response.status == 1) {
                    var modal = '<div id="transferBackupModal" class="modal fade" tabindex="-1" role="dialog" aria-labelledby="Transfer Backup" aria-hidden="true">' +
                        '<div class="modal-dialog">' +
                        '<div class="modal-content">' +
                        '<div class="modal-header">' +
                        '<h4 class="modal-title">Transfer Backup</h4>' +
                        '</div>' +
                        '<div class="modal-body">' +
                        '<p>This is a destructive action, if any matching schemas already exist they will be deleted, please make a backup first if required.</p>' +
                        '<p>Please select the destination database:</p>' +
                        '<select class="form-control" id="transferBackupModalSelect"></select>' +
                        '</div>' +
                        '<div class="modal-footer">' +
                        '<button type="button" class="btn btn-default" data-dismiss="modal">Close</button>' +
                        '<button type="button" class="btn btn-primary" id="transferBackupModalSaveButton">Transfer</button>' +
                        '</div>' +
                        '</div>' +
                        '</div>' +
                        '</div>';

                    $('body').append(modal);
                    
                    if(response.result) {
                        response.result.forEach(function(row){
                            $('#transferBackupModalSelect').append('<option value="' + row['id'] + '">' + row['title'] + ' (' + row['id'] + ')</option>');
                        });
                    }
                    
                    $('#transferBackupModalSaveButton').off('click');
                    $('#transferBackupModalSaveButton').on('click', function(){
                        transferBackup(id, $('#transferBackupModalSelect').val());
                    });

                    $('#transferBackupModal').modal('show');
                } else {
                    alert('Something went wrong, please try again.');
                }
            } else {
                alert('Something went wrong, please try again.');
            }
        })
        .fail(function() {
            // Tell user error
            alert('Something went wrong, please try again.');
        });
    }
}