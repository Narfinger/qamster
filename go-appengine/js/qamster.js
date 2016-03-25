var app = angular.module('qamsterApp', ['ngMaterial']);

// themes
app.config(function($mdThemingProvider) {
    $mdThemingProvider.theme('default')
        .primaryPalette('green');
});


app.controller('QamsterCtrl', ['$scope', '$mdSidenav', '$http', function($scope, $mdSidenav, $http){
    $scope.toggleSidenav = function(menuId) {
        $mdSidenav(menuId).toggle();
   };
    
    $scope.tasks = [];
    
    $scope.running = "not Running";
    $scope.tracking = 'Tracking Task';
    $scope.time = 'The time thing';

    updateTimeRunning($scope, $http);
    updateRunning($scope, $http);
    
    $scope.addTask = function () {
        t = document.getElementById("taskfield").value;
        console.log("added" + t);
        $http.post('/go/addTask', t);
        updateTimeRunning($scope, $http);
    }

    $scope.stop = function () {
        $http.post('/go/stop');
        updateTimeRunning($scope, $http);
        updateRunning($scope, $http);
    }
    
}]);

function updateTimeRunning($scope, $http) {
    updateTime($scope, $http);
    updateRunning($scope, $http);
}

    
function updateTime($scope, $http) {
    $http.get('/go/timetable').
        success(function(data) {
            $scope.tasks = data;
        });
}

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
