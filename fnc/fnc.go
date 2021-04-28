package main

import (
	"encoding/csv"
	"fmt"
	"io"
	"log"
	"os"
	"strconv"
)

func usage() {
	fmt.Fprintf(os.Stderr, "usage: %s [-d date] file\n", os.Args[0]);
	os.Exit(1);
}

func main() {
	var path, date string;
	var n, loss, gain, total float64;
	var arglen int;
	var dflag bool;

	arglen = len(os.Args);
	if arglen - 1 == 0 {
		usage();
	}
	path = os.Args[arglen - 1];
	for i := 0; i < arglen; i++ {
		if os.Args[i] == "-d" {
			date = os.Args[i+1];
			dflag = true;
		} else if os.Args[i] == "-h" {
			usage();
		}
	}

	f, err := os.Open(path);
	if err != nil {
		log.Fatal(err);
	}
	defer f.Close();

	csvr := csv.NewReader(f);
	if err != nil {
		log.Fatal(err);
	}
	header, err := csvr.Read();
	if err != nil {
		log.Fatal(err);
	}
	fmt.Println(header);

	for {
		line, err := csvr.Read();
		if err != nil {
			if err == io.EOF {
				break;
			}
			log.Fatal(err);
		}
		if dflag && date != line[0] {
			continue;
		}
		n, err = strconv.ParseFloat(line[2], 64);
		if err != nil {
			log.Fatal(err);
		}
		if n >= 0 {
			gain += n;
		} else {
			loss += n;
		}
		total += n;
		fmt.Printf("%-15s %-20s %-.2f\n", line[0], line[1], n);
	}
	fmt.Println();
	fmt.Printf("Loss:  %.2f\n", -loss);
	fmt.Printf("Gain:  %.2f\n", gain);
	fmt.Printf("Total: %.2f\n", total);
}
