var app = angular.module('qamsterApp', ['ngMaterial']);

// themes
app.config(function($mdThemingProvider) {
    $mdThemingProvider.theme('default')
        .primaryPalette('green');
});


app.controller('QamsterCtrl', ['$scope', '$mdSidenav', '$http', '$timeout', '$interval', '$mdToast', function($scope, $mdSidenav, $http, $timeout, $interval, $mdToast){
    $scope.toggleSidenav = function(menuId) {
        $mdSidenav(menuId).toggle();
    };
    $scope.tasks = [];
    $scope.summary = [];

    $scope.running = 'not Running';
    $scope.tracking = 'Tracking Task';
    $scope.time = '0';
    $scope.runningtimemin = 0;
    $scope.min_update_promise = null;

    $scope.selectedItem;
    $scope.searchText;
    
    $scope.refresh = function () {
        $http.get('/go/timetable').
            success(function(data) {
                // for(var i=0; i<data.length; i++) {
                //     var elem = data[i];
                //     data[i]['duration']=$scope.computeDuration(elem['start'], elem['end']);
                // }
                $scope.tasks = data;
            })
        $http.get('/go/statusbar').
            success(function(data) {
                $scope.summary = data;
                console.log(data)
            });
    };
    $scope.addTaskByString = function(string) {
        $scope.tracking = string;
        console.log('started: ' + string);
        
        $scope.task= null;
        $scope.tracking = "";
        $http.post('/go/addTask', string);
        $timeout(function(n) {
            $scope.refresh();
            updateRunning($scope, $http);
        }, 1000);

        $scope.runningtimemin = 0;
        $scope.min_update_promise =  $interval(function() {
            $scope.runningtimemin = $scope.runningtimemin + 1;
            $scope.time = $scope.secondsToTime($scope.runningtimemin * 60);}, 60*1000);

        $scope.showSimpleToast(string);
    }

    $scope.addTaskByTask = function(task) {
        var nstring = task.title;
        if (task.category!='')
            nstring = nstring + '@' + task.category;
        $scope.addTaskByString(nstring);
    }

    $scope.addTask = function () {
        console.log($scope.selectedItem);
        console.log($scope.searchText);
        if($scope.selectedItem!=null)
            t = $scope.selectedItem.title;
        else
            t = $scope.searchText;

        $scope.addTaskByString(t);
    }

    $scope.stop = function () {
        $http.post('/go/stop');
        $interval.cancel($scope.min_update_promise);
        $timeout(function(n) { $scope.refresh();
                               updateRunning($scope, $http);}, 1000);
        
    }

    $scope.secondsToTime = function(sec) {
        //var sec = (en-st) % 60;
        var min =  Math.round(  (sec/60) % 60);
        var hour = Math.round( (min/60) % 24);
        
        if (hour==0) {
            return min.toString().concat(" min");
        } else {
            return hour.toString().concat(
                ":".concat(min.toString().concat(" h")));
        }
    }


    $scope.computeDuration = function(start, end) {
        var st = new Date(start).valueOf();
        var en = new Date(end).valueOf();
        
        return $scope.secondsToTime( (en-st) % 60);
    }

    $scope.getMatchingTasks = function(input) {
        return $http.get('/go/searchTask?q=' + input).then(function(result) {return result.data;});
    };
    
        
    $scope.showSimpleToast = function(string) {
        var s = 'Started: ' + string;
        $mdToast.show(
            $mdToast.simple().textContent(s).hideDelay(3000)
    );
  };

        updateRunning($scope, $http);
        $scope.refresh();
}]);

function updateRunning($scope, $http) {
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
}
