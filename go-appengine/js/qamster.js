var app = angular.module('qamsterApp', ['ngMaterial']);

// themes
app.config(function($mdThemingProvider) {
    $mdThemingProvider.theme('default')
        .primaryPalette('green');
});


app.controller('QamsterCtrl', ['$scope', '$mdSidenav', '$http', '$timeout', '$interval', function($scope, $mdSidenav, $http, $timeout, $interval){
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
    
    $scope.refresh = function () {
        $http.get('/go/timetable').
            success(function(data) {
                for(var i=0; i<data.length; i++) {
                    var elem = data[i];
                    data[i]['duration']=$scope.computeDuration(elem['start'], elem['end']);
                }
                $scope.tasks = data;
            })
        $http.get('/go/statusbar').
            success(function(data) {
                $scope.summary = data;
                console.log(data)
            });
    };
    
    $scope.addTask = function () {
        t = document.getElementById('taskfield').value;
        console.log('added ' + t);
        $scope.tracking = t;
        $scope.task= null;
        $scope.tracking = "";
        $http.post('/go/addTask', t);
        $timeout(function(n) {
            $scope.refresh();
            updateRunning($scope, $http);
        }, 1000);

        $scope.runningtimemin = 0;
        $scope.min_update_promise =  $interval(function() {
            $scope.runningtimemin = $scope.runningtimemin + 1;
            $scope.time = $scope.secondsToTime($scope.runningtimemin * 60);}, 60*1000);
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

