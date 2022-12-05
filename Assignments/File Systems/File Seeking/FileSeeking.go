/*
	Name: 		Byron Dowling	
	Class: 		5143 Advanced Operating Systems
	Semester: 	Fall 2022
	Keyword Byte position
		- intro 	20
		- pass 		40
		- fail 		60
		- next 		80
 */

package main

import (
	"fmt"
	"os" 	// os package includes the Go Package "unix"   
)

// Handling Go's error responses 
func errorChecking(errResponse error) {
	if (errResponse != nil) {
		panic(errResponse)
	}
}

// Function that reads 20 bytes at a time afer the seek position has been set
func fileSeeking(FileIn *os.File) {

	stringSeek := make([]byte, 20)
	slice, err := FileIn.Read(stringSeek)
	errorChecking(err)
	fmt.Printf(string(stringSeek[:slice]))
	fmt.Println()
}


func main() {

	Fin, err := os.Open("input.txt")
	errorChecking(err)					// If the file fails to open

	for true {
		var text string

		fmt.Println("Waiting for input...Type STOP to end the program")
		fmt.Scanf("%s", &text)

		if (text == "Intro" || text == "intro") {

			var seekIndex int64 = 20
			_, err = Fin.Seek(seekIndex, 0)			// Seek to index 20
			errorChecking(err)
			fileSeeking(Fin)
		}

		if (text == "Pass" || text == "pass") {

			var seekIndex int64 = 40
			_, err = Fin.Seek(seekIndex, 0)			// Seek to index 40
			errorChecking(err)
			fileSeeking(Fin)
		}

		if (text == "Fail" || text == "fail") {

			var seekIndex int64 = 60
			_, err = Fin.Seek(seekIndex, 0)			// Seek to index 60
			errorChecking(err)
			fileSeeking(Fin)
		}

		if (text == "Next" || text == "next") {

			var seekIndex int64 = 80
			_, err = Fin.Seek(seekIndex, 0)			// Seek to index 80
			errorChecking(err)
			fileSeeking(Fin)
		}

		if (text == "STOP" || text == "Stop" || text == "stop") {
			break
		}
	}

	defer Fin.Close()
}
