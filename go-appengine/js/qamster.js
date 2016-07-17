'use strict';

var app = angular.module('qamsterApp', ['ngMaterial', 'ngRoute',]);

// themes
app.config(function($mdThemingProvider) {
    $mdThemingProvider.theme('default')
        .primaryPalette('green');
});

app.controller('SideNavController', function($scope, $mdSidenav, $route) {
    $scope.openMenu = function() {
        console.log("hit");
        $mdSidenav('left').toggle();
    };
    $scope.close = function () { $mdSidenav('left').close(); };
    $scope.main = function() { window.location = "/#";           $scope.close(); };
    $scope.history = function() { window.location = "/#history"; $scope.close()};
    
});

app.controller('EditDialogController',function($scope, $mdDialog, title, start, end) {
    $scope.title = title;
    $scope.start = start;
    $scope.end   = end;
    $scope.answer = function () {
        $mdDialog.hide();
    };
    $scope.answer = function(answer) {
        $mdDialog.hide(answer);
  };
});

app.controller('QamsterCtrl',function($scope, $mdSidenav, $http, $timeout, $interval, $mdToast, $mdDialog){ 
    $scope.tasks = [];
    $scope.summary = [];

    $scope.running = 'not Running';
    $scope.tracking = 'Tracking Task';
    $scope.time = '0';
    $scope.runningtimemin = 0;
    $scope.min_update_promise = null;

    $scope.searchedText = '';
    $scope.itemedText = '';
    
    
    $scope.refresh = function () {
        $http.get('/go/timetable').
            success(function(data) {
                // for(var i=0; i<data.length; i++) {
                //     var elem = data[i];
                //     data[i]['duration']=$scope.computeDuration(elem['start'], elem['end']);
                // }
                $scope.tasks = data;
            });
        $http.get('/go/statusbar').
            success(function(data) {
                $scope.summary = data;
//                console.log(data)
            });
    };
    $scope.addTaskByString = function(string) {
        $scope.tracking = string;
        $scope.time = '0';
        $scope.runningtimemin = 0;
        console.log('started: ' + string);
        
        $scope.task = null;
        $scope.tracking = "";
        $http.post('/go/addTask', string);
        $scope.showSimpleToast(string);
    };

    $scope.addTaskByTask = function(task) {
        var nstring = task.title;
        if (task.category!='')
            nstring = nstring + '@' + task.category;
        $scope.addTaskByString(nstring);
    };

    $scope.addTask = function () {
        $scope.addTaskByString($scope.searchedText);
    };

    $scope.addTaskFromChannel = function(task) {
        $scope.tracking = task.title;
        $scope.runningtimemin = 0;
        $scope.time = '0';
        $scope.min_update_promise =  $interval(function() {
            $scope.runningtimemin = $scope.runningtimemin + 1;
            $scope.time = $scope.secondsToTime($scope.runningtimemin * 60);}, 60*1000);

        $scope.updateRunning($scope, $http);
        
        //reset field
        $scope.searchedText = null;
        $scope.itemedText = null;
    };
    
    $scope.stop = function () {
        $http.post('/go/stop');
    };

    //only works for h<24
    $scope.secondsToTime = function(sec) {
        var min =  Math.round(  (sec/60) % 60);
        var hour = Math.floor( (sec/3600) % 24);
        
        if (hour==0) {
            return min.toString().concat(" min");
        } else {
            var mstring = min.toString();
            if (min<10 && min >0) {
                mstring = "0" + mstring;
            }
            return hour.toString().concat(
                ":".concat(mstring.concat(" h")));
        }
    };


    $scope.computeDuration = function(start, end) {
        var st = new Date(start).valueOf();
        var en = new Date(end).valueOf();
        
        return $scope.secondsToTime( (en-st) /1000);
    };

    $scope.getMatchingTasks = function(input) {
        return $http.get('/go/searchTask?q=' + input).then(function(response) {
            if (response.data=='null\n') //this is kind of a hack but I don't know why I need it
                return [];
            else
                return response.data;});
    };
    
        
    $scope.showSimpleToast = function(string) {
        var s = 'Started: ' + string;
        $mdToast.show(
            $mdToast.simple().textContent(s).hideDelay(3000)
        );
    };

    $scope.reloadPage = function() {
        location.reload();
        //$route.reload();
    };

    $scope.createSocket = function(channel) {
        var socket = channel.open();
        socket.onopen = function() {console.log("opened channel");};
        socket.onclose = function() {
            console.log("trying to reopen channel");
            $scope.createSocket(channel);
        };
        socket.onerror = function(err) {console.log("some error");};
        socket.onmessage = $scope.channelMsg;
    };
    
    $scope.createChannel = function() {
        $http.get('/go/createchannel').success(function(data) {
            var channel = new goog.appengine.Channel(data);
            $scope.createSocket(channel);
        });
    };

    $scope.channelMsg = function(d) {
        //console.log("msg: " + JSON.stringify(d.data));
        var msg = JSON.parse(d.data);
        console.log("message recieved");
        console.log(msg);
        if (msg.message=="addtask") {
            console.log("started task from channel");
            $scope.addTaskFromChannel(msg.task);
        } else if (msg.message=="stoptask") {
            console.log("stopped task from channel");
            $interval.cancel($scope.min_update_promise);
            $timeout(function(n) { $scope.refresh();
                                   $scope.updateRunning($scope, $http);}, 1000);
        }
    };

    $scope.onCloseChannel = function() {
        console.log("channel closed");
    };


    $scope.updateRunning = function($scope, $http) {
        $http.get('/go/running').
            success(function(data) {
                if (data.IsRunningField) {
                    $scope.running = "Running";
                } else {
                    $scope.running = "Not Running";
                    $scope.tracking = "";
                    return;
                }
                var task = data.RunningTask;
                $scope.tracking = task.title;
            });
    };
    
    $scope.editPage = function($ev, $title, $start, $end) {
        $mdDialog.show({
            controller: 'EditDialogController',
            templateUrl: 'editdialog.html',
            parent: angular.element(document.body),
            locals: { title: $title,
                      start: $start,
                      end: $end
                    },
//            targetEvent: ev,
            clickOutsideToClose:true,
        })
            .then(function(answer) {
                console.log(answer)
                console.log("accept");
                $scope.status = 'You said the information was "' + answer + '".';
            }, function() {
                console.log("reject");
                $scope.status = 'You cancelled the dialog.';
            });
    };

    //connect to channel
    $scope.createChannel();
    $scope.updateRunning($scope, $http);
    $scope.refresh();

});


app.controller('HistoryCtrl',function($scope, $mdSidenav, $http, $timeout, $interval, $mdToast){
    $scope.toggleSidenav = function(menuId) {
        $mdSidenav(menuId).toggle();
    };
});

app.config(function($routeProvider) {
    $routeProvider.when('/', {
        controller: 'QamsterCtrl',
        templateUrl: 'mainview.html',
    }).when('/history', {
        controller: 'HistoryCtrl',
        templateUrl: 'historyview.html',})
});
