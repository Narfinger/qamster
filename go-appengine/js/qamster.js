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

    updateTimeTable($scope, $http);
    updateRunning($scope, $http);
    
    $scope.addTask = function () {
        t = document.getElementById("taskfield").value;
        console.log("added " + t);
        $http.post('/go/addTask', t);
        updateTimeTable($scope, $http);
        updateRunning($scope, $http);
        
        $scope.running = "";
    }

    $scope.stop = function () {
        $http.post('/go/stop');
        updateTimeTable($scope, $http);
        updateRunning($scope, $http);
    }   
}]);
    
function updateTimeTable($scope, $http) {
    console.log("time table update called");
    $http.get('/go/timetable').
        success(function(data) {
            //console.log(data)
            //console.log(typeof(data))
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
