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
    
    updateTime($scope, $http);


    
    $scope.addTask = function () {
        t = document.getElementById("taskfield").value;
        console.log("added" + t);
        $http.post('/go/addTask', t);
        updateTime($scope, $http);
        updateRunning($scope, $http);
    }

    $scope.stop = function () {
        $http.post('/go/stop');
        $scope.$apply();
    }
    
}]);

function updateTime($scope, $http) {
    $http.get('/go/timetable').
        success(function(data) {
            $scope.tasks = data;
        });
}

function updateRunning($scope, $http) {
    $http.get('/go/isRunning').
        success(function(data) {
            if (data) {
                $scope.running = "Running";
            } else {
                $scope.running = "Not Running";
            }
        });
}
