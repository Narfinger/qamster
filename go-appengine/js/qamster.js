var app = angular.module('qamsterApp', ['ngMaterial']);

// themes
app.config(function($mdThemingProvider) {
    $mdThemingProvider.theme('default')
        .primaryPalette('green');
});


app.controller('QamsterCtrl', ['$scope', '$mdSidenav', '$http', '$timeout', function($scope, $mdSidenav, $http, $timeout){
    $scope.toggleSidenav = function(menuId) {
        $mdSidenav(menuId).toggle();
   };
    
    $scope.tasks = [];
    
    $scope.running = "not Running";
    $scope.tracking = 'Tracking Task';
    $scope.time = 'The time thing';

    $scope.refresh = function () {
        $http.get('/go/timetable').
            success(function(data) {
                $scope.tasks = data;
            })
    };
    
    $scope.addTask = function () {
        t = document.getElementById("taskfield").value;
        console.log("added " + t);
        $scope.tracking = t;
        $scope.task= null;
        $scope.tracking = "";
        $http.post('/go/addTask', t);
        $timeout(function(n) {
            $scope.refresh();
            updateRunning($scope, $http);
        }, 1000);
    }

    $scope.stop = function () {
        $http.post('/go/stop');
        $timeout(function(n) { $scope.refresh();
                               updateRunning($scope, $http);}, 1000);
        
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
