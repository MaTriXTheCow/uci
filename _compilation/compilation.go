package main

import (
	"bytes"
	"fmt"
	"log"
	"os/exec"
)

func main() {
	var out bytes.Buffer

	cmd := exec.Command(
		"cl",
		"/W4",
		"/O2",
		"/EHsc",
		"/std:c++20",
		"/Fo\".\\obj\\\"",
		"/Isrc",
		"/Isrc\\Logger",
		"/Isrc\\Board",
		"/Isrc\\ImageReader",
		"/Isrc\\InputProcess",
		"/Isrc\\Util",
		"/Isrc\\WindowProcess",
		"src\\main.cpp",
		"src\\uci.cpp",
		"src\\Logger\\logger.cpp",
		"src\\Board\\board.cpp",
		"src\\ImageReader\\imageReader.cpp",
		"src\\InputProcess\\inputProcess.cpp",
		"src\\WindowProcess\\WindowProcess.cpp",
		"src\\Util\\util.cpp",
		"user32.lib",
		"Kernel32.lib",
		"/link",
		"/out:uci.exe",
		"/SUBSYSTEM:CONSOLE",
		"/ENTRY:wWinMainCRTStartup",
	)

	cmd.Stdin = nil
	cmd.Stdout = &out

	err := cmd.Run()

	if err != nil {
    fmt.Printf("%s", out.String())
		log.Fatal(err)
	}

	fmt.Printf("%s\n", out.String())
}